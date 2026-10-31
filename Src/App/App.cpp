#include "App.hpp"
#if __has_include("Servo.hpp")
#include "Servo.hpp"
#elif __has_include("../Servo/Servo.hpp")
#include "../Servo/Servo.hpp"
#elif __has_include("../Servo.hpp")
#include "../Servo.hpp"
#elif __has_include("App/Servo.hpp")
#include "App/Servo.hpp"
#elif __has_include("../App/Servo.hpp")
#include "../App/Servo.hpp"
#else
#include "Servo.hpp"
#endif
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