#pragma once
#include "Singleton.hpp"
#include "sdkconfig.h"

#if EXTENSION_ET08A
#include "ET08A.hpp"
#include "ServoPwm.hpp"
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

private:
    std::unique_ptr<ET08A> receiver_;
    std::unique_ptr<ServoPwm> lxServo_;
    static constexpr int16_t STICK_CHANNEL   = 2;
    
    static constexpr int16_t STICK_SLEW_STEP = 4;   // 4 count/ms ≈ 0.54°/ms ≈ 537°/s

    
    //int16_t lastStick_ = 0;  
  

#endif

};