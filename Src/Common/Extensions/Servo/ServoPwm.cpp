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

    /* 遥控器量程 [-671, 670] 先归一化到角度 0~180，再走 setAngle 的同一套映射 */
    setAngle((rc - RC_MIN) * 180.0f / static_cast<float>(RC_MAX - RC_MIN));
}

void ServoPwm::setAngle(float _deg)
{
    /* 限幅，防止超范围把舵机顶死 */
    _deg = std::clamp(_deg, 0.0f, 180.0f);

    /* 角度 → CCR：CCR = 500 + angle/180 * 2000（文档 4-5 节公式） */
    const uint16_t ccr =
        static_cast<uint16_t>(PULSE_MIN_US + _deg / 180.0f * (PULSE_MAX_US - PULSE_MIN_US));
    writePulse(ccr);
}

void ServoPwm::setCenter()
{
    setAngle(90.0f);
}
