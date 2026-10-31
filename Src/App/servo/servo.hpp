#pragma once

#include "Bsp_pwm.hpp"

/**
 * @brief 50Hz 舵机（SG90 / MG996R）
 *        20ms 周期，0.5ms~2.5ms 高电平 -> 0°~180°
 */
class Servo {
public:
    static constexpr uint32_t MIN_PULSE_US = 500;  // 0°
    static constexpr uint32_t MAX_PULSE_US = 2500; // 180°

    Servo(TIM_HandleTypeDef *_htim, uint32_t _channel);

    void start();

    void stop();

    /**
     * @brief set servo angle
     * @param _angle 0~180，超出自动限幅
     */
    void setAngle(uint16_t _angle);

private:
    Pwm pwm_;
};
