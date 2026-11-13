#pragma once

#include "MotorMsgs.hpp"
#include "Bsp_can.hpp"

namespace MOTOR {

class DJIMotor {
public:
    //param
    
    static constexpr float CURRENT_MAX = 3.0f;
    static constexpr float CURRENT_CODE_MAX = 16384.0f;

    DJIMotor() = default;

    void init(canHandle *_hcan, uint8_t _id);

    bool cmdCurrent(float _txData);

private:
    /* GM6020 电流控制帧的标准帧 ID：
     *   0x1FE 控电机 ID 1~4，0x2FE 控电机 ID 5~7
     * 依据《GM6020 使用说明》"CAN 通信协议"一节；
     * 电流给定范围 -16384~16384 对应 -3A~3A，与上面 CURRENT_MAX 一致。
     * （若改用电压控制则是 0x1FF / 0x2FF，范围 -25000~25000） */
    /* ★ 本题填写（原骨架：两个常量都写成 = 0） */
    static constexpr uint32_t TX_STDID_1_4 = 0x1FE;   // ★填：电流帧，控 ID 1~4
    static constexpr uint32_t TX_STDID_5_7 = 0x2FE;   // ★填：电流帧，控 ID 5~7

    uint16_t currentToCode(float _current);
    uint32_t txStdID();

    uint32_t lastTxMs_ = 0;
    DJIRegInfo_s regInfo_ = {};
};

} // namespace MOTOR