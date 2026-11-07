#include "servo.h"

static TIM_HandleTypeDef *tim = 0;
static uint32_t channel = TIM_CHANNEL_1;

void Servo_Init(TIM_HandleTypeDef *htim, uint32_t ch)
{
    tim = htim;
    channel = ch;

    __HAL_TIM_SET_COMPARE(tim, channel, 1500);
    HAL_TIM_PWM_Start(tim, channel);
}

void Servo_SetAngle(float angle)
{
    if (tim == 0) {
        return;
    }

    if (angle < 0.0f) {
        angle = 0.0f;
    } else if (angle > 180.0f) {
        angle = 180.0f;
    }

    uint16_t ccr = (uint16_t)(SERVO_MIN_CCR +
                              angle / 180.0f * (SERVO_MAX_CCR - SERVO_MIN_CCR) + 0.5f);

    __HAL_TIM_SET_COMPARE(tim, channel, ccr);
}

void Servo_Sweep(float from, float to, uint16_t period_ms)
{
    if (to == from) {
        return;
    }

    float step = (to > from) ? 1.0f : -1.0f;

    uint32_t steps = (uint32_t)((to > from) ? (to - from) : (from - to));

    uint32_t delay_ms = period_ms / steps;
    if (delay_ms == 0) {
        delay_ms = 1;
    }

    for (float a = from; (step > 0.0f) ? (a <= to) : (a >= to); a += step) {
        Servo_SetAngle(a);
        HAL_Delay(delay_ms);
    }
}
