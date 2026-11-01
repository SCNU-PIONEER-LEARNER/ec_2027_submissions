#pragma once
#include "Singleton.hpp"
#include "sdkconfig.h"
#include "main.h"
#include "ServoPwm.hpp"

#if EXTENSION_ET08A
#include "ET08A.hpp"
#include <memory>
#endif

class App : public Singleton<App> {
    // pwm呼吸灯、转舵机，串口，电机pid测试，遥控器上层实现，考核框架
public:
    void init();

    void task();

#if EXTENSION_ET08A
    bool readRcData(ET08A::RcData_s &_data);
    uint32_t receivedRcFrames() const;
#endif

private:
    void sweep(uint32_t _now);

    /* F103：TIM3_CH1 硬件 PWM 驱动舵机（CubeMX 里 PSC=71 / ARR=19999） */
    ServoPwm servo_{&htim3, TIM_CHANNEL_1};

    float angle_ = 90.0f;
    float dir_ = -1.0f;
    uint32_t lastStepMs_ = 0;

#if EXTENSION_ET08A
    std::unique_ptr<ET08A> receiver_;
    std::unique_ptr<ServoPwm> lxServo_;
#endif
};
