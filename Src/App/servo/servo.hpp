#pragma once
#include "Bsp_pwm.hpp"

class Servo
{
public:
    Servo(TIM_HandleTypeDef* htim, uint32_t channel);
    void setAngle(float angle);
    float getAngle() const;

private:
    Pwm pwm_;
    float angle_;
};