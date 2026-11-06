#include "ServoPwm.hpp"
#include <algorithm>

ServoPwm::ServoPwm(TIM_HandleTypeDef *_timer, uint32_t _channel) : timer_(_timer), channel_(_channel) {}

HAL_StatusTypeDef ServoPwm::init()
{
    if (timer_ == nullptr) {
        return HAL_ERROR;
    }

    /* 先输出中位脉冲再启动 PWM，避免上电瞬间舵机打到极限 */
    __HAL_TIM_SET_COMPARE(timer_, channel_, PULSE_CENTER_US);
    currentPulseUs_ = PULSE_CENTER_US;

    const HAL_StatusTypeDef status = HAL_TIM_PWM_Start(timer_, channel_);
    initialized_ = (status == HAL_OK);
    return status;
}

void ServoPwm::setFromRc(int16_t _value)
{
    if (!initialized_ || timer_ == nullptr) {
        return;
    }

    int32_t value = _value;
    if (value < RC_MIN) {
        value = RC_MIN;
    } else if (value > RC_MAX) {
        value = RC_MAX;
    }

    const int32_t pulse =
        static_cast<int32_t>(PULSE_MIN_US)
        + (value - RC_MIN) * static_cast<int32_t>(PULSE_MAX_US - PULSE_MIN_US)
              / static_cast<int32_t>(RC_MAX - RC_MIN);

    currentPulseUs_ = static_cast<uint16_t>(pulse);
    __HAL_TIM_SET_COMPARE(timer_, channel_, currentPulseUs_);
}

void ServoPwm::setCenter()
{
    if (!initialized_ || timer_ == nullptr) {
        return;
    }
    currentPulseUs_ = PULSE_CENTER_US;
    __HAL_TIM_SET_COMPARE(timer_, channel_, currentPulseUs_);
}
