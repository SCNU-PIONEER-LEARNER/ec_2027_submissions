#include "App.hpp"
#include "Algorithm/Filter.hpp"
#include "FreeRTOS.h"
#include "Algorithm.hpp"
#include "GM6020.hpp"
#include "ET08A.hpp"
#include "Motor/Projdefs.hpp"
#include "PID.hpp"
#include <cstdint>
#include <math.h>
using namespace std;
MotorData_s motortest;
//uint32_t rxBuffer0;
ET08A::RcData_s RcData;


volatile float   g_actVel   = 0.f; // 实际转速 rpm（单环反馈，LPF 20Hz 后的值）
volatile float   g_refVel   = 0.f; // 期望转速 rpm（单环给定，可在 FreeMASTER 里写）
//volatile uint8_t g_fmEnable = 0;   // 1 = 期望转速改由 FreeMASTER 给（g_refVel 生效）

PositionalPid  velPid (0.925f, 0.0001f, 0.0000001f, 0.001f, 10.f, 12.f);
void App::init()
{
    //注意：框架中控制频率为1000Hz，即采样周期为1ms
    InitConfig_s motorConfig = { .cmdType = CmdType_e::OFF, .txBaseId = 0x1FF, .rxBaseId = 0x204, .offsetId = 1 };
    motor = new GM6020(motorConfig);
    motorPid.posPid = new PositionalPid(0.f, 0.f, 0.f, 0.001f, 0.f, 132.f);
    motorPid.velPid = new PositionalPid(0.f, 0.f, 0.0f, 0.001f, 0.f, 24.f);
    velPid_torq.velPid = new PositionalPid(0.0f, 0.0f, 0.0f, 0.001f, 10.f, 1.2f);
    velPid_volt.velPid = new PositionalPid(0.0f, 0.0f, 0.0f, 0.001f, 10.f, 24.f);
    iirFilter = new FILTER::LPFIIR3(500.f, 20.f, FILTER::FilterType_e::BUTTERWORTH, FILTER::Ripple_e::NONE);
    torqFilter = new FILTER::LPF(1000.f, 1.f);
#if APP_USE_RC
    et08a = new ET08A(&huart3);
    et08a->start();
    refvel = 0;
#endif
}

App::~App() { refvel = 0; }

void App::task() {
    update();//采样：CAN 反馈 + 遥控帧 + 期望值；挂档解析 → ctrlMode / rcEnable / refAng / refvel

#if APP_USE_RC
    if (!rcEnable) {                     // 下档（或遥控离线）：停机
        stopMotor();
        return;
    }
#endif

    if (ctrlMode == CtrlMode_e::SINGLE_VEL) {
        velCtrl();                       // 中档：单环速度环
    } else {
        motorCtrl(refAng);               // 上档：双环（位置环 + 速度环）
    }
}

void App::update()
{   //电机反馈：一帧 CAN（CAN1，ID 0x204，8 字节）
    motorCtrlData.Ang = motor->getMotorData().Ang;                             // rad,  [0, 2π)
    motorCtrlData.Vel = iirFilter->process(motor->getMotorData().Vel);
    motortest = motor->getMotorData();   // rpm
   //遥控输入：25 字节帧到RcData_s
#if APP_USE_RC
    et08a->processRxQueue();
    et08a->updateFormatDetection();
    et08a->recoverIfOffline();

    const ET08A::RcData_s &rcData = et08a->getData();
   RcData  = et08a->getData();
    switch (rcData.SB) {
        case ET08A::Sw_e::DOWN:                       // 停
            rcEnable = false;
            refvel   = 0.f;
            refAng   = motorCtrlData.Ang;             // 记住当前角度，下次上档从此处保持
            break;

        case ET08A::Sw_e::MID:                        // 单环
            ctrlMode = CtrlMode_e::SINGLE_VEL;
            rcEnable = true;
            refvel   = 30.f + parseRC(rcData.ch2);    // 摇杆给期望转速
            break;

        case ET08A::Sw_e::UP:                         // 双环
            ctrlMode = CtrlMode_e::DUAL_LOOP;
            rcEnable = true;
            refAng   = rangeMap(refAng + parseRC(rcData.ch1), 0.f, 2 * M_PI);
            break;

        default:
            rcEnable = false;
            break;
    }

    //切环 / 重新使能时清 PID，避免积分残留
    if (rcEnable != lastEnable || ctrlMode != lastMode) {
        resetPids();
    }
    lastEnable = rcEnable;
    lastMode   = ctrlMode;
#endif

    /* FreeMASTER: 置 1 后期望转速改由 g_refVel 给定 */
//     if (g_fmEnable) {
//         refvel = g_refVel;
//     }
}

float App::parseRC(int16_t _ch) { return (static_cast<float>(_ch) * 0.002f / 1341.f); }

void App::stopMotor()
{
    g_refVel = 0.f;
    g_actVel = motorCtrlData.Vel;
    motor->disable();                    // cmdType = OFF，发送全 0 帧，电机自由停转
}

void App::velCtrl()
{
    motor->setCmdType(CmdType_e::SET_VOLT); //电机的"指令类型"设成电压模式
    g_refVel = refvel;                      // FreeMASTER 观测：期望转速
    g_actVel = motorCtrlData.Vel;           // FreeMASTER 观测：实际转速
    float out = velPid.positionalCalc(refvel, motorCtrlData.Vel); //期望转速，实际转速
    /*pid输出值有小数；*/
    motor->cmd(out); //发送结果
}
void App::motorCtrl(float _refAng){
    float errAng =getMinorArc(_refAng, motorCtrlData.Ang);// _refAng期望角度
    motorPid.refvel=motorPid.posPid->positionalCalc(errAng, 0.f);//调用位置环的 PID 计算,外环
    //motorPid.refvel —— 把这个计算结果存起来，它就是"下一步该用多快的转速"。
    float outVolt = motorPid.velPid->positionalCalc(motorPid.refvel, motorCtrlData.Vel); //内环
     motor->setCmdType(CmdType_e::SET_VOLT);  //把电机对象里那个 cmdType 变量改成 SET_VOLT
    motor->cmd(outVolt); //发帧 

}
void App::resetPids() {
    motorPid.posPid->reset();
    motorPid.velPid->reset();
    velPid_torq.velPid->reset();
    velPid_volt.velPid->reset();
}
