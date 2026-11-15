#pragma once

#include "Bsp.hpp"

extern "C" {
extern TIM_HandleTypeDef htim3;   /* 由 CubeMX 在 main.c 中定义 */
}

/**
 * @brief 舵机驱动（SG90/MG996R，50Hz，0.5~2.5ms 对应 0~180°）
 */
class Servo {
public:
    Servo(TIM_HandleTypeDef *_htim, uint32_t _channel) : pwm_(_htim, _channel) {}

    void init();

    void setAngle(float _deg);

    float angle() const { return angle_; }

private:
    Pwm pwm_;
    float angle_ = 90.0f;
};