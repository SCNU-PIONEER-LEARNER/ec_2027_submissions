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

void App::init() { servo_.start(); }

void App::task()
{
    static int angle = 0;
    static int dir = 2;
    static uint8_t divider = 0;

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

#endif

    if (++divider < 20) { // AppManager 每 1ms 调一次，累计 20ms 更新一次角度
        return;
    }
    divider = 0;

    servo_.setAngle(static_cast<uint16_t>(angle));

    angle += dir;
    if (angle >= 180) {
        angle = 180;
        dir = -2;
    } else if (angle <= 0) {
        angle = 0;
        dir = 2;
    }
}

#if EXTENSION_ET08A
bool App::readRcData(ET08A::RcData_s &_data) {}

#endif
