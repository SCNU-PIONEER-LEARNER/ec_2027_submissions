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

#if EXTENSION_ET08A
#include "main.h"
#endif

void App::init() { servo_.init(); /* HAL_TIM_PWM_Start(TIM3_CH1) + 回中位 90° */ }

/* 90° -> 0° -> 90° 往返，每 22ms 走 1°，单程约 2s。
 * 不能用文档里的 HAL_Delay：App::task() 由 AppManager 每 1ms 调一次，阻塞会卡死整个任务 */
void App::sweep(uint32_t _now)
{
    if (_now - lastStepMs_ < 22) {
        return;
    }
    lastStepMs_ = _now;

    angle_ += dir_;
    if (angle_ >= 90.0f) {
        angle_ = 90.0f;
        dir_ = -1.0f;
    } else if (angle_ <= 0.0f) {
        angle_ = 0.0f;
        dir_ = 1.0f;
    }
    servo_.setAngle(angle_);
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

    sweep(HAL_GetTick());
}

#if EXTENSION_ET08A
bool App::readRcData(ET08A::RcData_s &_data) {}

#endif
