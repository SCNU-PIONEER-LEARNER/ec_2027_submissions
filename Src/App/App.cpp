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


void App::init()
{
    receiver_ = std::make_unique<ET08A>(&huart3);
    if (receiver_->start() != HAL_OK) {
        LOG::error("App", "ET08A start failed");
    }

    lxServo_ = std::make_unique<ServoPwm>(&htim1, TIM_CHANNEL_2);
    if (lxServo_->init() != HAL_OK) {
        LOG::error("App", "ServoPwm init failed");
    }

}

void App::task()
{
    if (lxServo_ == nullptr || receiver_ == nullptr) {
        return;
    }

    /* 必须周期性消费 DMA 收到的字节并驱动格式探测，否则永远拼不出帧 */

    receiver_->processRxQueue();
    receiver_->updateFormatDetection();   /* 轮询格式探测，推进探测状态机 */

    const bool online = receiver_->isOnline();

    if (!online) {
        lxServo_->setCenter();
        const HAL_StatusTypeDef recoveryStatus = receiver_->recoverIfOffline();
        if (recoveryStatus != HAL_OK && recoveryStatus != HAL_BUSY) {
            LOG::error("App", "ET08A UART DMA recovery failed: %d", recoveryStatus);
        }
        return;
    }

    const ET08A::RcData_s data = receiver_->getData();
    lxServo_->setFromRc(data.ch2);   /* 用哪个通道取决于接线/习惯，可按需改 */
}

bool App::readRcData(ET08A::RcData_s &_data)
{
    if (receiver_ == nullptr) {
        return false;
    }
    _data = receiver_->getData();
    return receiver_->isOnline();
}

uint32_t App::receivedRcFrames() const
{
    return receiver_ != nullptr ? receiver_->receivedFrames() : 0U;
}
