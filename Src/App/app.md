#include "App.hpp"
#include "Algorithm/Filter.hpp"
#include "FreeRTOS.h"
#include "Algorithm.hpp"
#include "GM6020.hpp"
#include "ET08A.hpp"
#include "Motor/Projdefs.hpp"
#include "PID.hpp"
#include <algorithm>
#include <cstdint>
#include <math.h>
using namespace std;
MotorData_s motortest;
ET08A::RcData_s rctest;
float g_motor_volt_cmd = 0.f;  
void App::init()
{
    InitConfig_s motorConfig = { .cmdType = CmdType_e::SET_VOLT, .txBaseId = 0x1FF, .rxBaseId = 0x204, .offsetId = 1 };
    motor = new GM6020(motorConfig);
    motorPid.posPid = new PositionalPid(110.0f, 4.0f, 0.f, 0.001f, 30.f, 132.f);
    // 速度环输出是电压(V)：限幅 ±25.2 V，积分限幅取一半
    motorPid.velPid = new PositionalPid(0.05f, 0.f, 0.0f, 0.001f, 12.6f, 25.2f);
    velPid_volt.velPid = new PositionalPid(0.05f, 0.02f, 0.0f, 0.005f, 12.6f, 25.2f);
    iirFilter = new FILTER::LPFIIR3(1000.f, 100.f, FILTER::FilterType_e::BUTTERWORTH, FILTER::Ripple_e::NONE);
    torqFilter = new FILTER::LPF(1000.f, 1.f);
    refvel = 0.f;
    refAng = 0.f;
#if APP_USE_RC
    et08a = new ET08A(&huart3);
    et08a->start();
#else
    mode = CtrlMode_e::POS_LOOP; 
#endif
}

App::~App() { refvel = 0; }

void App::task()
{
    update(); 
    g_motor_volt_cmd = 0.f;   
    if (mode == CtrlMode_e::POS_LOOP && prevMode != CtrlMode_e::POS_LOOP)
    {
        refAng = motorCtrlData.Ang;
    }
    prevMode = mode;

#if APP_USE_RC
    // if (!et08a->isOnline())
    // {
    //     motor->disable();
    //     return;
    // }
#endif

    switch (mode) 
    {
    case CtrlMode_e::STOP:
        motor->disable(); 
        return;
    case CtrlMode_e::VEL_LOOP: 
        velCtrl();
        break;
    case CtrlMode_e::POS_LOOP: 
        motorCtrl(refAng);
        break;
    }

    sendCtrlData();
}

void App::update()
{
    const MotorData_s data = motor->getMotorData();
    motortest = data;
    rctest = et08a->getData();
    motorCtrlData.Ang = data.Ang;
    motorCtrlData.Vel = iirFilter->process(data.Vel);

#if APP_USE_RC
    et08a->processRxQueue();
    et08a->updateFormatDetection();
    et08a->recoverIfOffline();

    if (!et08a->isOnline())
    {
        return;
    }

    const ET08A::RcData_s &rcData = et08a->getData();
    rctest = rcData;

    if (!modeLocked)
    {
        switch (rcData.SB)
        {
        case ET08A::Sw_e::DOWN:
            mode = CtrlMode_e::STOP;
            refvel = 0.f;
            break;
        case ET08A::Sw_e::MID:
            mode = CtrlMode_e::VEL_LOOP;
            break;
        case ET08A::Sw_e::UP:
            mode = CtrlMode_e::POS_LOOP;
            break;
        }
    }

    if (mode == CtrlMode_e::VEL_LOOP)
    {
        refvel = 30.f;
        velPid_volt.refvel = refvel;
    }
    else if (mode == CtrlMode_e::POS_LOOP)
    {
        refAng = rangeMap(refAng + parseRC(rcData.ch1), 0.f, 2 * M_PI);
    }
#endif
}

void App::motorCtrl(float _refang)
{
    const float refAngNow = motorCtrlData.Ang + getMinorArc(_refang, motorCtrlData.Ang);
    motorPid.refvel = motorPid.posPid->positionalCalc(refAngNow, motorCtrlData.Ang);
    motorPid.out = motorPid.velPid->positionalCalc(motorPid.refvel, motorCtrlData.Vel);
}

void App::velCtrl()
{
    velPid_volt.out = velPid_volt.velPid->positionalCalc(velPid_volt.refvel, motorCtrlData.Vel);

}

void App::sendCtrlData()
{
    const bool posLoop = (mode == CtrlMode_e::POS_LOOP);
    const float out = posLoop ? motorPid.out : velPid_volt.out;

    g_motor_volt_cmd = out;
    motor->cmd(out);
}

// 遥控器通道值 -> 每毫秒的角度增量（满杆约 1 rad/s）
float App::parseRC(int16_t _ch) { return (static_cast<float>(_ch) * 0.002f / 1341.f); }