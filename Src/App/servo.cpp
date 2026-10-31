/* ================= servo.c ================= */
#include "main.h"
#include "servo.hpp"
#include "tim.h"



#ifndef SERVO_MIN_CCR
#define SERVO_MIN_CCR 500   // 对应 0.5ms (0度)
#endif
#ifndef SERVO_MAX_CCR
#define SERVO_MAX_CCR 2500  // 对应 2.5ms (180度)
#endif





/* 初始化：启动 PWM，并把舵机放到中位 */
void Servo_Init(void)
{
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);   // 启动 TIM3_CH1 PWM
    Servo_SetAngle(90.0f);                      // 中位 90°，避免上电猛转
}

/* 设置舵机角度：angle 范围 0~180 */
void Servo_SetAngle(float angle)
{
    /* 限幅，防止超范围把舵机顶死 */
    if (angle < 0.0f)   angle = 0.0f;
    if (angle > 180.0f) angle = 180.0f;

    /* 角度 → CCR：CCR = 500 + angle/180 * 2000 */
    uint16_t ccr = (uint16_t)(SERVO_MIN_CCR + angle / 180.0f * (SERVO_MAX_CCR - SERVO_MIN_CCR));

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, ccr);
}

/* 平滑地从当前角度转到目标角度 */
void Servo_Sweep(float from, float to, uint16_t period)
{
    float step = (to > from) ? 1.0f : -1.0f;   // 每次变 1°
    float delay = step * period / (to - from);  // 由周期计算每步停顿时间
    for (float a = from; (step > 0) ? (a <= to) : (a >= to); a += step)
    {
        Servo_SetAngle(a);
        HAL_Delay(100);      // 每步之间的时间，控制速度
    }
}