/*
 * @Author: userName userEmail
 * @Date: 2026-10-28 17:33:14
 * @LastEditors: userName userEmail
 * @LastEditTime: 2026-10-30 21:24:50
 * @FilePath: \leaner\ec_2027_submissions\Src\Common\Extensions\Servo\ServoPwm.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include "Soc.hpp"
#include HAL_INCLUDE
#include <cstdint>

class ServoPwm final {
public:
    static constexpr int16_t RC_MIN = -671;
    static constexpr int16_t RC_MAX = 670;

    static constexpr uint16_t PULSE_MIN_US = 500;
    static constexpr uint16_t PULSE_CENTER_US = 1550;
    static constexpr uint16_t PULSE_MAX_US = 2598;

    ServoPwm(TIM_HandleTypeDef *_timer, uint32_t _channel);

    HAL_StatusTypeDef init();
    void setFromRc(int16_t _value);
    void setCenter();//具体函数名自行更改

private:
    TIM_HandleTypeDef *timer_;
    uint32_t channel_;
    uint16_t currentPulseUs_ = PULSE_CENTER_US;
    bool initialized_ = false;
};
/*C_MIN = -671 / RC_MAX = 670	遥控器杆量的有效范围，与 ET08A 解码出来的 ±1024-ish 偏移量对齐
PULSE_MIN_US = 500	舵机最小脉宽（µs）
PULSE_CENTER_US = 1550	中位脉宽
PULSE_MAX_US = 2598	最大脉宽
timer_ / channel_	用哪个定时器、哪个通道（本项目是 TIM3 + TIM_CHANNEL_1）
currentPulseUs_	上一次设定的脉宽，默认中位
initialized_	是否已经 init() 过*/