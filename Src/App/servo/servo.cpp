#include "servo.hpp"

Servo::Servo(TIM_HandleTypeDef *_htim, uint32_t _channel) : pwm_(_htim, _channel) {}

void Servo::start()
{
    pwm_.setDutyCycle(MIN_PULSE_US); // 先给有效脉宽，避免上电乱抖
    pwm_.start();
}

void Servo::stop() { pwm_.stop(); }

void Servo::setAngle(uint16_t _angle)
{
    if (_angle > 180U) {
        _angle = 180U;
    }
    pwm_.setDutyCycle(MIN_PULSE_US + static_cast<uint32_t>(_angle) * (MAX_PULSE_US - MIN_PULSE_US) / 180U);
}
