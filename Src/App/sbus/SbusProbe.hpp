#pragma once

/**
 * @brief SBUS / W.BUS 串口诊断探针（临时用）
 *
 * 挂在 USART3 上，逐字节接收后拼成 25 字节帧，
 * 每 500ms 通过 RTT 打印一次：帧头、帧尾、通道值、HAL 错误码。
 *
 * 判读方法：
 *   b0  == 15 (0x0F) 且 b24 == 0  → 帧对齐成功，校验参数正确
 *   ch0~ch3 在 172~1811 之间        → 11 位解包正确
 *   err != 0                        → 见 HAL_UART_ERROR_* 位定义
 *                                     PE=1 校验错 / NE=2 噪声 / FE=4 帧错 / ORE=8 溢出
 *
 * 验证完成后，删掉本文件与 App.cpp 里的两处调用即可。
 */
class SbusProbe {
public:
    static SbusProbe &instance();

    /** 启动接收（在 App::init 中调用一次） */
    void init();

    /** 每 1ms 调用一次 */
    void poll();

private:
    SbusProbe() = default;
};
