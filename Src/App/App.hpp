#pragma once
#include "Projdefs.hpp"
#include "Singleton.hpp"
#include "PID.hpp"
#include"GM6020.hpp"
#include "ET08A.hpp"
#include "Filter.hpp"
#include "Bsp_uart.hpp"
#include "stm32f4xx_hal_uart.h"
struct MotorCtrlData_s {
    float Ang;
    float Vel;
};
struct MotorCtrl_s {
    PositionalPid *posPid;
    PositionalPid *velPid;
    float refvel;
    float torq;
};

class App : public Singleton<App> {
    // pwm呼吸灯、转舵机，串口，电机pid测试，遥控器上层实现，考核框架
    static constexpr uint32_t BUFFER_LEN = 128;
public:
    void init();

    void task();

    void update();
    void motorCtrl(float _refang);
    void velCtrl();
    void sendCtrlData();
    float parseRC(int16_t _ch);

    MotorCtrlData_s motorCtrlData;
    MotorCtrl_s motorPid;
    MotorCtrl_s velPid_torq;
    MotorCtrl_s velPid_volt;
private:
    ~App();
    friend class Singleton<App>;
    Uart *uart;
    //uint8_t *rxBuf;
    bool dataReady = false;
    GM6020 *motor;
    ET08A *et08a;
    FILTER::LPFIIR3 *iirFilter;
    FILTER::LPF *torqFilter;
    

    float refvel;
    float refAng;

    void uartRecv(UART_HandleTypeDef *huart,uint16_t size);
};