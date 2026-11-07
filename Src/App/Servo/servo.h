#ifndef SERVO_H
#define SERVO_H

#include "main.h"

#define SERVO_MIN_CCR   500
#define SERVO_MAX_CCR   2500

void Servo_Init(TIM_HandleTypeDef *htim, uint32_t ch);
void Servo_SetAngle(float angle);
void Servo_Sweep(float from, float to, uint16_t period_ms);

#endif
