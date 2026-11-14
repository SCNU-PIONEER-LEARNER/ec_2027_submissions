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
    float Vel;//实测速度
};
struct MotorCtrl_s {
    PositionalPid *posPid;//位置环
    PositionalPid *velPid;//速度环
    float refvel;//速度给定
    float torq;//力矩
};
enum class CtrlMode_e : uint8_t{SINGLE_VEL,DUAL_LOOP,};//单环，双环
class App : public Singleton<App> {
    // pwm呼吸灯、转舵机，串口，电机pid测试，遥控器上层实现，考核框架
    static constexpr uint32_t BUFFER_LEN = 128;
public:
    void init();

    void task();

    void update();
    void motorCtrl(float _refang);
    void velCtrl();
    void sendCtrlData();         //唯一输出出口
    void resetPids();             //切环/使能时清 PID
    float parseRC(int16_t _ch);

    MotorCtrlData_s motorCtrlData;
    MotorCtrl_s motorPid;//双环用
    MotorCtrl_s velPid_torq;
    MotorCtrl_s velPid_volt;//单环控制组
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
    CtrlMode_e ctrlMode ;// = CtrlMode_e::SINGLE_VEL;//现在挂在哪一档
    CtrlMode_e lastMode ;// = CtrlMode_e::SINGLE_VEL;//上一拍挂在哪一档
    bool rcEnable        = false;//现在不许转
    bool lastEnable      = false;//上一拍不许转
    
    float refvel = 0.f;
    float refAng = 0.f;

    void uartRecv(UART_HandleTypeDef *huart,uint16_t size);
   
};


