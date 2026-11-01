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

    /* 按《外设与寄存器》4-5 节：0~180 度对应 CCR 500~2500，中位 1500 */
    static constexpr uint16_t PULSE_MIN_US = 500;
    static constexpr uint16_t PULSE_CENTER_US = 1500;
    static constexpr uint16_t PULSE_MAX_US = 2500;

    ServoPwm(TIM_HandleTypeDef *_timer, uint32_t _channel);

    HAL_StatusTypeDef init();
    void setFromRc(int16_t _value);
    void setAngle(float _deg);
    void setCenter();//具体函数名自行更改

private:
    void writePulse(uint16_t _us);

    TIM_HandleTypeDef *timer_;
    uint32_t channel_;
    uint16_t currentPulseUs_ = PULSE_CENTER_US;
    bool initialized_ = false;
};
