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
    // DJI 电调电流控制帧的标准帧 ID：ID 1~4 共用 0x200，ID 5~8 共用 0x1FF
    static constexpr uint32_t TX_STDID_1_4 = 0x200;
    static constexpr uint32_t TX_STDID_5_7 = 0x1FF;

    uint16_t currentToCode(float _current);
    uint32_t txStdID();

    uint32_t lastTxMs_ = 0; //上次发送电流指令的时间
    DJIRegInfo_s regInfo_ = {};//这台电机怎么发"的通讯注册信息
};

} // namespace MOTOR