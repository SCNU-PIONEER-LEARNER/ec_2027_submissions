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

    if (imuInstance == nullptr || !imuInstance->isReady()) {
        angle_ = {};
        motorCurrent_ = 0.0f;
        motor.ctrl(0.0f);
        return;
    }
    angle_ = imuInstance->getData();
    // TODO:
    //static const uint32_t t0 = HAL_GetTick();
    //const uint32_t ms = HAL_GetTick() - t0;
    //const float targetCurrent = ((ms % 2000U) < 1000U) ? 0.5f : -0.5f;
    // “前 1 秒给正电流，后 1 秒给负电流，一直循环”。
   // const float targetCurrent = std::clamp(0.0f, -MAX_CURRENT, MAX_CURRENT);
   constexpr float TARGET_PITCH = 0.0f;   /* 目标角度：回到水平 */
    constexpr float KP = 0.02f;            /* 比例系数：每 1 度出多少安培 */
    constexpr float DEAD_BAND = 0.5f;      /* 死区：偏差小于它就不出力 */

    const float pitch = angle_.pitch;         /* 当前角度（度） */
    const float error = TARGET_PITCH - pitch; /* 偏差 = 目标 - 当前 */

    float targetCurrent = 0.0f;               /* 默认不出力 */
    if (error > DEAD_BAND) {
        targetCurrent = KP * error;           /* 歪了，往一个方向出力 */
    } else if (error < -DEAD_BAND) {
        targetCurrent = KP * error;           /* 歪了，往反方向出力 */
    }
    motorCurrent_ = std::clamp(targetCurrent, motorCurrent_ - MAX_CURRENT_STEP,
                               motorCurrent_ + MAX_CURRENT_STEP);
    motorCurrent_ = std::clamp(targetCurrent, - MAX_CURRENT,
                                MAX_CURRENT);
    motor.ctrl(motorCurrent_);
#endif
}