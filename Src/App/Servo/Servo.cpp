#include "Servo.hpp"
#include "FreeRTOS.h"
#include "task.h"

namespace {
constexpr uint32_t kMinPulseUs = 500;    //   0° → 0.5ms
constexpr uint32_t kMaxPulseUs = 2500;   // 180° → 2.5ms
constexpr uint32_t kUsPerCount = 10;     // 100kHz 定时器：1 计数 = 10us
}

void Servo::init()
{
    pwm_.setPrescaler(719);
    pwm_.start();
    setAngle(0);
    lastTick_ = xTaskGetTickCount();
}

void Servo::update()
{
    const uint32_t now = xTaskGetTickCount();      // configTICK_RATE_HZ=1000 → 1 tick = 1ms

    if (now - lastTick_ < kStepMs) {
        return;                                    // 没到 1 秒立刻返回 —— 关键：绝不阻塞
    }
    lastTick_ = now;

    const uint8_t next = static_cast<uint8_t>(angle_ + kStepDeg);
    setAngle(next > 180 ? 0 : next);               // 180+90 越界 → 回 0
}

void Servo::setAngle(uint8_t angle)
{
    if (angle > 180) {
        angle = 180;
    }
    angle_ = angle;

    const uint32_t us = kMinPulseUs
                      + static_cast<uint32_t>(angle) * (kMaxPulseUs - kMinPulseUs) / 180U;

    pwm_.setDutyCycle(us / kUsPerCount);           // 换算成 CCR
}