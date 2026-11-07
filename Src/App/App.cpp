/*
 * @Author: userName userEmail
 * @Date: 2026-10-28 15:35:27
 * @LastEditors: userName userEmail
 * @LastEditTime: 2026-10-30 21:31:20
 * @FilePath: \leaner\ec_2027_submissions\Src\App\App.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "App.hpp"
#include "FreeRTOS.h"//FreeRTOS 类型/宏
#include "StmLog.hpp"
#include <cstdint>

#if EXTENSION_ET08A
#include "main.h"
#endif

void App::init() {
#if EXTENSION_ET08A
    receiver_ = std::make_unique<ET08A>(&SBUS_UART); // 创建接收机对象
    receiver_->start(); // 启动接收：内部创建静态消息
    //队列、给 UART 注册 DMA 空闲回调（HAL_UARTEx_RxEventCallback）和错误重启回调，然后用 HAL_UARTEx_ReceiveToIdle_DMA

    lxServo_ = std::make_unique<ServoPwm>(&htim1, TIM_CHANNEL_2); // 创建舵机对象：TIM1 通道 2 -> PE11 引脚
    const HAL_StatusTypeDef servoStatus = lxServo_->init(); // 初始化舵机 PWM
    if (servoStatus != HAL_OK) {
        // 时基配置错误时 PWM 不会启动，必须让上层看得见，否则表现为"代码正常但舵机不动"
        LOG::error("App", "ServoPwm init failed: %d", servoStatus);
    }
#endif
}

void App::task()
{
#if EXTENSION_ET08A
    if (lxServo_ == nullptr || receiver_ == nullptr) {
        return;//空指针保护：init() 没成功（或宏关闭）时直接返回，避免解引用空指针
    }

    receiver_->processRxQueue(); // 把 DMA 收到的帧搬进解析流程

    // 判定“接收机是否在线”：收到过帧 且 距最后一帧 ≤ 100ms
    if (!receiver_->isOnline()) {
        // 离线分支：舵机回中 + 尝试恢复 DMA
        lxServo_->setCenter();
       // lastStick_ = 0;

        const HAL_StatusTypeDef recoveryStatus = receiver_->recoverIfOffline();
        if (recoveryStatus != HAL_OK && recoveryStatus != HAL_BUSY) {
            LOG::error("App", "ET08A UART DMA recovery failed: %d", recoveryStatus);
        }
        return;
    }

    // 在线分支：取遥控杆量 → 斜率限幅 → 换算脉宽 → 写 CCR
    const ET08A::RcData_s rc = receiver_->getData();

    /* 本函数由 AppManager 以 1 ms 周期调用，每周期最多变化 STICK_SLEW_STEP 个 count，
     * 杆量突然打满时舵机也是匀速跟过去，不会因为阶跃指令打齿或拉大电流。 
    if (target > static_cast<int16_t>(lastStick_ + STICK_SLEW_STEP)) {
        lastStick_ = static_cast<int16_t>(lastStick_ + STICK_SLEW_STEP);
    } else if (target < static_cast<int16_t>(lastStick_ - STICK_SLEW_STEP)) {
        lastStick_ = static_cast<int16_t>(lastStick_ - STICK_SLEW_STEP);
    } else {
        lastStick_ = target; // 差值在一步之内，直接对齐，避免中位抖动
    }

    lxServo_->setFromRc(lastStick_);*/
#endif
}

#if EXTENSION_ET08A//App 对外的两个查询接口（数据出口 + 诊断出口）
bool App::readRcData(ET08A::RcData_s &_data) {
   if (receiver_ == nullptr) {
        return false;//空指针检查，未初始化返回 false
    }
    _data = receiver_->getData();
    return receiver_->isOnline(); // RcData_s 无 failsafe 字段，在线即视为有效
}

uint32_t App::receivedRcFrames() const
{
    return receiver_ != nullptr ? receiver_->receivedFrames() : 0;
}

#endif
/*App.cpp
├─ #if EXTENSION_ET08A
│   └─ #include "main.h"          ← 为了拿到 SBUS_UART / htim1 的句柄声明
│
├─ App::init()                    组装：造对象 + 启动外设        (只在开机跑一次)
│    ├─ make_unique<ET08A>(&SBUS_UART)     + start()
│    └─ make_unique<ServoPwm>(&htim1, CH2) + init()      (PE11)
│
├─ App::task()                    调度：1 kHz 循环体              (每 1ms 跑一次)
│    ├─ 空指针双检
│    ├─ processRxQueue() → 解析帧
│    ├─ if (!online) → 回中 + 尝试恢复 DMA
│    └─ else → 取杆量 → SLEW 限幅 → setFromRc()
│
└─ #if EXTENSION_ET08A
    ├─ App::readRcData()          出口：给外部一份遥控数据快照
    └─ App::receivedRcFrames()    出口：诊断计数器*/