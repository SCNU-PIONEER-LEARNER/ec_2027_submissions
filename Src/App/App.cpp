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
#include "usercode.h"

#if EXTENSION_ET08A
#include "main.h"
#endif

void App::init()
{
    UserCode_Init();
    servo_.init();  /* HAL_TIM_PWM_Start(TIM3_CH1) + 回中位 */
}

void App::task()
{
#if EXTENSION_ET08A
    if (lxServo_ == nullptr || receiver_ == nullptr) {
        return;
    }

    const bool online = receiver_->isOnline();

    if (!online) {
        lxServo_->setCenter();
        const HAL_StatusTypeDef recoveryStatus = receiver_->recoverIfOffline();
        if (recoveryStatus != HAL_OK && recoveryStatus != HAL_BUSY) {
            LOG::error("App", "ET08A UART DMA recovery failed: %d", recoveryStatus);
        }
    }

    return;
#endif

    /* 无遥控器时的自测：照文档 Servo_Sweep(0, 90, 2) 的节奏，0° ↔ 90° 往返。
       每 22ms 走 1°，单程约 2s；不能用文档里的 HAL_Delay（会卡死 AppTask） */
    static uint32_t lastMs = 0;
    static float angle = 90.0f;  /* 与 init() 的中位对齐，避免上电先跳一下 */
    static float dir = -1.0f;

    const uint32_t now = HAL_GetTick();
    if (now - lastMs < 22) {
        return;
    }
    lastMs = now;

    angle += dir;
    if (angle >= 90.0f) {
        angle = 90.0f;
        dir = -1.0f;
    } else if (angle <= 0.0f) {
        angle = 0.0f;
        dir = 1.0f;
    }

    servo_.setAngle(angle);
}

#if EXTENSION_ET08A
bool App::readRcData(ET08A::RcData_s &_data) {}

#endif
