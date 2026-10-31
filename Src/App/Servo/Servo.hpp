#pragma once

#include "Bsp.hpp"
#include <cstdint>

extern TIM_HandleTypeDef htim3; 

class Servo {
public:
    void init();
    void update();                 // 每 1ms 调用一次，不阻塞
    void setAngle(uint8_t angle);

private:
    static constexpr uint32_t kStepMs  = 1000;   // 每 1000ms 走一步
    static constexpr uint8_t  kStepDeg = 90;     // 每步 90°

    Pwm      pwm_{&htim3, TIM_CHANNEL_1};        // 用框架的 PWM 封装
    uint8_t  angle_    = 0;
    uint32_t lastTick_ = 0;
};