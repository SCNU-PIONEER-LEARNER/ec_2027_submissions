#include "App.hpp"
#include "DJIMotor.hpp"
#include "Soc.hpp"
#include "MotorManager.hpp"
#include "FreeRTOS.h"
#include "main.h"
#include <algorithm>

#if CAN_ASSESSMENT_ADVANCED
static INS::Ins *imuInstance = nullptr;
#endif

void App::init()
{
    motor.init();
#if CAN_ASSESSMENT_ADVANCED
    imuInstance = &INS::Ins::instance();
    imuInstance->init(&HCAN2);
#endif
}


void App::task()
{
    motor.ctrl(0.5f);
    return;

#if CAN_ASSESSMENT_BASIC
    motor.ctrl(CAN_BASIC_CURRENT_MA / 1000.0f);//决定电机的电流大小，单位为A
#elif CAN_ASSESSMENT_ADVANCED
    constexpr float MAX_CURRENT = 1.0f;
    constexpr float MAX_CURRENT_STEP = 0.01f;

    if (imuInstance == nullptr || !imuInstance->isReady()) {
        angle_ = {};
        motorCurrent_ = 0.0f;
        motor.ctrl(0.0f);
        return;
    }
    angle_ = imuInstance->getData();
    // TODO:
    float targetCurrent = 0.0f;
    if(angle_.yaw > 2.0f){
        targetCurrent = -0.5f;
} else if(angle_.yaw < -2.0f){
    targetCurrent = 0.5f;
}
    motor.ctrl(targetCurrent);
#else
    motorCurrent_ = 0.0f;
    motor.ctrl(0.0f);
#endif
}