#include "App.hpp"
#include "Servo.hpp"
#include "FreeRTOS.h"

static Servo servo;

void App::init()
{
    servo.init();
}

void App::task()
{
    servo.update();
}