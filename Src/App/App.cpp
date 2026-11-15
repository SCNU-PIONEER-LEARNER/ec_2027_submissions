#include "App.hpp"
#include "DJIMotor.hpp"
#include "Soc.hpp"
#include "MotorManager.hpp"
#include "FreeRTOS.h"
#include "main.h"
#include <algorithm>
#include <cmath>
#include "Bsp_can.hpp"

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
    constexpr float KP = 0.02f;              
    constexpr float YAW_DEADBAND_DEG = 1.0f; 

    if (imuInstance == nullptr || !imuInstance->isReady())
    {
        angle_ = {};
        motorCurrent_ = 0.0f;
        yawLocked_ = false; // 掉线后重新锁零位
        motor.ctrl(0.0f);   // 没数据就不给力，安全第一
        return;
    }
    
    angle_ = imuInstance->getData();

    if (!yawLocked_) 
    {          
        targetYaw_ = angle_.yaw; // 把当前朝向记成目标
        yawLocked_ = true;
    }

    // TODO: 角度误差 → 目标电流(P 控制)
    float error = targetYaw_ - angle_.yaw;
    while (error > 180.0f)
    {
        error -= 360.0f; 
    }
    
    while (error < -180.0f) 
    {
        error += 360.0f;
    }
    
    if (std::fabs(error) < YAW_DEADBAND_DEG) 
    {
        error = 0.0f; 
    }

    const float targetCurrent = std::clamp(KP * error, -MAX_CURRENT, MAX_CURRENT);
    motorCurrent_ = std::clamp(targetCurrent, motorCurrent_ - MAX_CURRENT_STEP,
                               motorCurrent_ + MAX_CURRENT_STEP);

    motor.ctrl(motorCurrent_);
#endif
}