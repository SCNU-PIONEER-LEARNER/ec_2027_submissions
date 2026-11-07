#include "ServoPwm.hpp"
#include <algorithm>
#include <cstdint>

namespace {

constexpr uint32_t SERVO_TICK_HZ = 1000000U; // 1 count = 1 µs
constexpr uint32_t SERVO_PERIOD_US = 20000U; // 20 ms -> 50 Hz 帧周期

static_assert(ServoPwm::PULSE_MAX_US < SERVO_PERIOD_US,
              "PULSE_MAX_US must be less than one servo period");

/* APB 预分频编码 → 实际分频系数：0xx = /1，100 = /2，101 = /4，110 = /8，111 = /16 */
uint32_t apbDivider(uint32_t _ppre)
{
    return (_ppre < 4U) ? 1U : (1UL << (_ppre - 3U));
}

/* 定时器内核时钟：APB 预分频不为 1 时，定时器时钟 = PCLK × 2（F4 的定时器倍频）
 * 注意 TIM1/TIM8/TIM9/TIM10/TIM11 挂在 APB2 上，不能一律用 PCLK1 推算。 */
uint32_t timerKernelClockHz(const TIM_TypeDef *_instance)
{
    const bool onApb2 = (_instance == TIM1) || (_instance == TIM8) || (_instance == TIM9) ||
                        (_instance == TIM10) || (_instance == TIM11);

    if (onApb2) {
        const uint32_t divider = apbDivider((RCC->CFGR & RCC_CFGR_PPRE2) >> RCC_CFGR_PPRE2_Pos);
        const uint32_t pclk2 = HAL_RCC_GetPCLK2Freq();
        return (divider == 1U) ? pclk2 : (pclk2 * 2U);
    }

    const uint32_t divider = apbDivider((RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos);
    const uint32_t pclk1 = HAL_RCC_GetPCLK1Freq();
    return (divider == 1U) ? pclk1 : (pclk1 * 2U);
}

} // namespace

ServoPwm::ServoPwm(TIM_HandleTypeDef *_timer, uint32_t _channel) : timer_(_timer), channel_(_channel) {}

HAL_StatusTypeDef ServoPwm::init()
{
    if (timer_ == nullptr || timer_->Instance == nullptr) {
        return HAL_ERROR; // 句柄无效，后面解引用会 HardFault
    }//空指针防护

    /* 由定时器真实内核时钟反算 PSC，保证「1 count = 1µs」。
     * 无论系统跑 HSI 16 MHz 还是 PLL 168 MHz、无论 TIMx 挂在 APB1 还是 APB2，
     * 这个换算都成立，不再依赖 PCLK1/(PSC+1) 那种只在特定时钟树下凑巧成立的前提。 */
    const uint32_t timerClock = timerKernelClockHz(timer_->Instance);
    if (timerClock == 0U || (timerClock % SERVO_TICK_HZ) != 0U) {
        return HAL_ERROR; // 时基无法整除到 1 MHz，µs 换算不成立
    }

    const uint32_t divider = timerClock / SERVO_TICK_HZ;
    if (divider == 0U || divider > 0x10000U) {
        return HAL_ERROR; // PSC 是 16 位，超出范围说明时钟配置不合理
    }

    const uint32_t prescaler = divider - 1U;
    const uint32_t period = SERVO_PERIOD_US - 1U; // 20 ms -> 50 Hz（CubeMX 里的 2000-1 是 500 Hz）

    if (timer_->Init.Prescaler != prescaler || timer_->Init.Period != period) {
        timer_->Init.Prescaler = prescaler;
        timer_->Init.Period = period;
        /* 重写 PSC/ARR 并产生一次更新事件，让新值立刻进入影子寄存器 */
        if (HAL_TIM_Base_Init(timer_) != HAL_OK) {
            return HAL_ERROR;
        }
    }

    __HAL_TIM_SET_COMPARE(timer_, channel_, currentPulseUs_);

    if (HAL_TIM_PWM_Start(timer_, channel_) != HAL_OK) {
        return HAL_ERROR;
    }//先写 CCR，再启动

    initialized_ = true;
    return HAL_OK;//成功标志
}

//把遥控器的一个有符号整数换算成舵机的脉宽（单位 µs），并立刻写入定时器的 CCR，舵机映射
void ServoPwm::setFromRc(int16_t _value)// 杆量 → 脉宽 → 写寄存器
{
    const int16_t value = std::clamp(_value, RC_MIN, RC_MAX);//限幅,d:\ec_2027_submissions\Lib\Eigen3\Eigen\src\Core\MathFunctionsImpl.h

    int32_t pulse = static_cast<int32_t>(PULSE_CENTER_US);//分段线性映射,y=y 0+k(x−x 0)
    if (value > 0) {
        /* +670 -> +1048us，落在 2598us */
        pulse += (static_cast<int32_t>(value) * (PULSE_MAX_US - PULSE_CENTER_US)) / RC_MAX;
    } else if (value < 0) {
        /* -671 -> -1050us，落在 500us；两端行程不等，必须分段 */
        pulse += (static_cast<int32_t>(value) * (PULSE_CENTER_US - PULSE_MIN_US)) / (-RC_MIN);
    }
    
    currentPulseUs_ = static_cast<uint16_t>(pulse);
    if (!initialized_) {
        return;
    }

    __HAL_TIM_SET_COMPARE(timer_, channel_, currentPulseUs_);
}

void ServoPwm::setCenter()//回中位
{
    currentPulseUs_ = PULSE_CENTER_US;
    if (!initialized_) {
        return;
    }

    __HAL_TIM_SET_COMPARE(timer_, channel_, currentPulseUs_);
}
