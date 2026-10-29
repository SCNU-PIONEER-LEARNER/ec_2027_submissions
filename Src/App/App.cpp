#include "App.hpp"
#include "FreeRTOS.h"
#include "servo/servo.hpp"
#include "tim.h"
#include "task.h"

Servo& servo()
{
    static Servo instance(&htim3, TIM_CHANNEL_1);
    return instance;
}
void App::init() 
{
    servo().setAngle(90.0F);
}

void App::task() 
{
    static uint32_t step=0;

    switch(step)
    {
        case 0:
            servo().setAngle(0.0F);
            break;
        case 1:
            servo().setAngle(90.0F);
            break;
        case 2:
            servo().setAngle(180.0F);
            break;
    }
    step++;
    if(step>=3)
    {
        step=0;
    }
    vTaskDelay(pdMS_TO_TICKS(4000));
}
