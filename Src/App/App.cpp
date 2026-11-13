#include "App.hpp"
#include "DJIMotor.hpp"
#include "Soc.hpp"
#include "MotorManager.hpp"
#include "FreeRTOS.h"
#include "main.h"
#include <algorithm>
#include <cmath>

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
    /* 必做：恒定电流，电机持续转动；同时打反馈日志证明收得到数据 */
    motor.ctrl(CAN_BASIC_CURRENT_MA / 1000.0f);
#else
    /* 进阶：用 IMU 姿态控制电机电流 */
    constexpr float MAX_CURRENT = 1.0f;       // 电流上限（A），保护电机与电源
    constexpr float MAX_CURRENT_STEP = 0.01f; // 每 1ms 最多变化 0.01A → 10A/s 斜坡
    constexpr float GAIN = 0.05f;             // 比例系数：俯仰 20° → 1A

    if (imuInstance == nullptr || !imuInstance->isReady()) {
        angle_ = {};
        motorCurrent_ = 0.0f;
        motor.ctrl(0.0f);
        return;
    }

    angle_ = imuInstance->getData();

    /* 倾角 → 目标电流：
     *   |pitch| < 2°    → 0（死区，避免静止时因噪声抖来抖去）
     *   否则            → pitch × 增益，再限幅到 ±MAX_CURRENT
     * 方向：pitch 为正（车头朝下）给正电流，反了就把 GAIN 取负号。 */
    float targetCurrent = 0.0f;
        targetCurrent = angle_.pitch * GAIN;
    targetCurrent = std::clamp(targetCurrent, -MAX_CURRENT, MAX_CURRENT);

    /* 斜坡限幅：不允许电流突跳，否则电机瞬间满转矩、电流冲击大 */
    motorCurrent_ = std::clamp(targetCurrent, motorCurrent_ - MAX_CURRENT_STEP,
                               motorCurrent_ + MAX_CURRENT_STEP);

    motor.ctrl(motorCurrent_);
#endif
}
