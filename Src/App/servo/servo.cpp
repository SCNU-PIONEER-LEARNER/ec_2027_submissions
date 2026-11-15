#include "servo.hpp"

void Servo::init()
{
    pwm_.start();
    setAngle(90.0f);  
}

void Servo::setAngle(float _deg)
{
    if (_deg < 0.0f) {
        _deg = 0.0f;
    }
    if (_deg > 180.0f) {
        _deg = 180.0f;
    }
    angle_ = _deg;

    const uint32_t period = pwm_.autoLoader() + 1U;          /* 20ms 对应的计数值 */
    const uint32_t minCcr = period / 40U;                    /* 0.5ms */
    const uint32_t maxCcr = period / 8U;                     /* 2.5ms */
    const uint32_t ccr = minCcr + static_cast<uint32_t>(_deg * (maxCcr - minCcr) / 180.0f);
    pwm_.setDutyCycle(ccr);
}