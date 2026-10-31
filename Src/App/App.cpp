#include "App.hpp"
#include "FreeRTOS.h"
#include "servo.hpp"
#include "task.h"

void App::init() {
    Servo_SetAngle(90.0F);  // 初始化舵机到中位
}

void App::task() {
    static uint32_t step=0;

    switch(step)
    {
        case 0:
            Servo_SetAngle(0.0F);
            break;
        case 1:
            Servo_SetAngle(90.0F);
            break;
        case 2:
            Servo_SetAngle(180.0F);
            break;
    }
    step++;
    if(step>=3)
    {
        step=0;
    }
}
