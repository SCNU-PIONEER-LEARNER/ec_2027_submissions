/*
 * @Author: userName userEmail
 * @Date: 2026-10-28 15:35:27
 * @LastEditors: userName userEmail
 * @LastEditTime: 2026-10-30 21:31:20
 * @FilePath: \leaner\ec_2027_submissions\Src\App\App.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "App.hpp"
#include "FreeRTOS.h"
#include "StmLog.hpp"
#include <cstdint>

#include "main.h"
#include "servo.h"

void App::init()
{
    Servo_Init(&htim3, TIM_CHANNEL_1);
}

void App::task()
{
    Servo_Sweep(0, 180, 3600);
    Servo_Sweep(180, 0, 3600);
}
