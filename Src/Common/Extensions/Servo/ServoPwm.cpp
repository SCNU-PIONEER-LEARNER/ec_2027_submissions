#include "ServoPwm.hpp"
#include <algorithm>

ServoPwm::ServoPwm(TIM_HandleTypeDef *_timer, uint32_t _channel) : timer_(_timer), channel_(_channel) {}

void ServoPwm::writePulse(uint16_t _us)
{
    /* CCR 直接写微秒值：前提是 CubeMX 里 Prescaler 配成 71（1MHz 计数） */
    __HAL_TIM_SET_COMPARE(timer_, channel_, _us);
    currentPulseUs_ = _us;
}

HAL_StatusTypeDef ServoPwm::init()
{
    const HAL_StatusTypeDef status = HAL_TIM_PWM_Start(timer_, channel_);
    if (status != HAL_OK) {
        return status;
    }

    setCenter();
    initialized_ = true;
    return HAL_OK;
}

void ServoPwm::setFromRc(int16_t _value)
{
    if (!initialized_) {
        return;
    }

    const int16_t rc = std::clamp(_value, RC_MIN, RC_MAX);

    /* 把遥控器量程 [RC_MIN, RC_MAX] 线性映射到脉宽 [PULSE_MIN_US, PULSE_MAX_US] */
    constexpr int32_t kRcSpan    = RC_MAX - RC_MIN;
    constexpr int32_t kPulseSpan = PULSE_MAX_US - PULSE_MIN_US;

    const int32_t offset = (static_cast<int32_t>(rc) - RC_MIN) * kPulseSpan;
    const int32_t pulse  = PULSE_MIN_US + (offset + kRcSpan / 2) / kRcSpan;

    writePulse(static_cast<uint16_t>(std::clamp(pulse, static_cast<int32_t>(PULSE_MIN_US),
                                               static_cast<int32_t>(PULSE_MAX_US))));
}

void ServoPwm::setCenter()
{
    writePulse(PULSE_CENTER_US);
}
