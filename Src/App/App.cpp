#include "App.hpp"
#include "Algorithm/Filter.hpp"
#include "FreeRTOS.h"
#include "Algorithm.hpp"
#include "GM6020.hpp"
#include "ET08A.hpp"
#include "Motor/Projdefs.hpp"
#include "PID.hpp"
#include "Bsp_Uart.hpp"
#include "Bsp_Dma.hpp"
#include <algorithm>

#include "stm32f4xx_hal_uart.h"
#include <cstdint>
#include <math.h>
using namespace std;

void App::init()
{
    //注意：框架中控制频率为1000Hz，即采样周期为1ms
    InitConfig_s motorConfig = { .cmdType = CmdType_e::SET_VOLT, .txBaseId = 0x1FF, .rxBaseId = 0x204, .offsetId = 1 };
    motor = new GM6020(motorConfig);
    motorPid.posPid = new PositionalPid(28.f, 3.f, 0.f, 0.001f, 0.f, 132.f);
    motorPid.velPid = new PositionalPid(0.55f, 0.f, 0.005f, 0.001f, 0.f, 1.2f);
    velPid_torq.velPid = new PositionalPid(0.01f, 0.003f, 0.001f, 0.001f, 10.f, 1.2f);
    velPid_volt.velPid = new PositionalPid(0.01f, 0.003f, 0.001f, 0.001f, 10.f, 1.2f);
    iirFilter = new FILTER::LPFIIR3(500.f, 20.f, FILTER::FilterType_e::BUTTERWORTH, FILTER::Ripple_e::NONE);
    torqFilter = new FILTER::LPF(1000.f, 1.f); //加史
    et08a = new ET08A(&huart3);
    et08a->start();
    refvel = 0;
}

App::~App() { refvel = 0; }

void App::task() {}

void App::update()
{
    motorCtrlData.Ang = motor->getMotorData().Ang;
    motorCtrlData.Vel = iirFilter->process(motor->getMotorData().Vel);

    et08a->processRxQueue();
    et08a->updateFormatDetection();
    et08a->recoverIfOffline();

    const ET08A::RcData_s &rcData = et08a->getData();
    if (rcData.SB == ET08A::Sw_e::DOWN) {
        refAng = 0.f;
        refvel = 0.f;
        motor->disable();
    }
    if (rcData.SB == ET08A::Sw_e::MID) {
        refvel = 30;
        refAng = rangeMap(refAng + parseRC(rcData.ch1), 0.f, 2 * M_PI);
    }
    if (rcData.SB == ET08A::Sw_e::UP) {
        refvel = 30;
        refAng = rangeMap(refAng + parseRC(rcData.ch1), 0.f, 2 * M_PI);
    }
}

float App::parseRC(int16_t _ch) { return (static_cast<float>(_ch) * 0.002f / 1341.f); }
