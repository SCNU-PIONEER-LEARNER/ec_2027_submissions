/* ================= servo.h ================= */
#ifndef __SERVO_H
#define __SERVO_H

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

void Servo_Init(void);
void Servo_SetAngle(float angle);
void Servo_Sweep(float from, float to, uint16_t period);


#define SERVO_MIN_CCR   500    // 0°   对应 0.5ms
#define SERVO_MAX_CCR   2500   // 180° 对应 2.5ms

void Servo_Init(void);
void Servo_SetAngle(float angle);

#ifdef __cplusplus
}
#endif

#endif