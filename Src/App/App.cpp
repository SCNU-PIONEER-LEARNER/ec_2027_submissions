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

    /* 无遥控器时的自测：0° ↔ 180° 往返，单程约 2s（20ms 走 13 个 RC 单位） */
    static uint32_t lastMs = 0;
    static int16_t rc = ServoPwm::RC_MIN;
    static int16_t dir = 1;

    const uint32_t now = HAL_GetTick();
    if (now - lastMs < 20) {
        return;
    }
    lastMs = now;

    rc = static_cast<int16_t>(rc + dir * 13);
    if (rc >= ServoPwm::RC_MAX) {
        rc = ServoPwm::RC_MAX;
        dir = -1;
    } else if (rc <= ServoPwm::RC_MIN) {
        rc = ServoPwm::RC_MIN;
        dir = 1;
    }

    servo_.setFromRc(rc);
}

#if EXTENSION_ET08A
bool App::readRcData(ET08A::RcData_s &_data) {}

#endif
