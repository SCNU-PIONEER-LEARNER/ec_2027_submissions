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
#if CAN_ASSESSMENT_BASIC
    motor.ctrl(CAN_BASIC_CURRENT_MA / 1000.0f);
#else
    constexpr float MAX_CURRENT = 1.0f;
    constexpr float MAX_CURRENT_STEP = 0.01f;
    //IMU 失效保护
    if (imuInstance == nullptr || !imuInstance->isReady()) {
        angle_ = {};
        motorCurrent_ = 0.0f;
        motor.ctrl(0.0f);
        return;
    }
    angle_ = imuInstance->getData();//取姿态
    // TODO:
    constexpr float TARGET_PITCH = 0.0f;    // 平衡点
    constexpr float I0 = 0.5f;              // 电流幅值（唯一的参数，符号决定方向）
    constexpr float DEADBAND = 1.0f;        // 目标附近不动作，防抖
   // motorCurrent_ = std::clamp(targetCurrent, motorCurrent_ - MAX_CURRENT_STEP,
    //                          motorCurrent_ + MAX_CURRENT_STEP);   //变化率限制
     const float err = TARGET_PITCH - angle_.pitch;
     motorCurrent_ = 0.0f;
    if (err > DEADBAND)       motorCurrent_ = -I0;
    else if (err < -DEADBAND) motorCurrent_ =  I0;

    motor.ctrl(motorCurrent_);//下发
#endif
}
