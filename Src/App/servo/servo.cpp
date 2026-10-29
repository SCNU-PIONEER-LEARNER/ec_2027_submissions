#include "servo/servo.hpp"

// TIM3: 72MHz/(71+1) = 1MHz 计数 -> 1 个 tick = 1us, Period=19999 -> 20ms (50Hz)
// 舵机脉宽 0.5ms~2.5ms 对应 CCR 500~2500
const uint32_t SERVO_MIN_CCR=500;
const uint32_t SERVO_MAX_CCR=2500;
const float SERVO_MIN_ANGLE=0.0F;
const float SERVO_MAX_ANGLE=180.0F;

Servo::Servo(TIM_HandleTypeDef* htim, uint32_t channel)
    : pwm_(htim, channel), angle_(0.0F)
{
    pwm_.start();
}

void Servo::setAngle(float angle)
{
    if (angle < SERVO_MIN_ANGLE) angle = SERVO_MIN_ANGLE;
    if (angle > SERVO_MAX_ANGLE) angle = SERVO_MAX_ANGLE;

    angle_ = angle;
    float ratio=(angle-SERVO_MIN_ANGLE)/(SERVO_MAX_ANGLE-SERVO_MIN_ANGLE);
    uint32_t ccr=SERVO_MIN_CCR+static_cast<uint32_t>(
        static_cast<float>(SERVO_MAX_CCR-SERVO_MIN_CCR)*ratio+0.5F);
    pwm_.setDutyCycle(ccr);
}

float Servo::getAngle() const
{
    return angle_;
}