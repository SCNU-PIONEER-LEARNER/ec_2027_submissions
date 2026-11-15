#include "DJIMotor.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>

using namespace MOTOR;

void DJIMotor::init(canHandle *_hcan, uint8_t _id)
{
    // DJI 电调 ID 有效范围为 1~8：1~4 使用 0x200，5~8 使用 0x1FF
    if (_hcan == nullptr || _id < 1 || _id > 8) {
        inited_ = false;
        return;
    }

    regInfo_.hcan = _hcan;
    regInfo_.id = _id;
    // 同一控制帧内，电机按 ID 顺序每台占用 2 字节
    regInfo_.txSlot = static_cast<uint8_t>((_id - 1) % MOTOR_NUM_PER_FRAME);
    lastTxMs_ = 0;
    inited_ = true;
}

bool DJIMotor::cmdCurrent(float _current)
{
    if (!inited_ || regInfo_.hcan == nullptr) {
        return false;
    }

    // 控制帧发送间隔不小于 1ms，避免总线拥塞
    const uint32_t now = HAL_GetTick();
    if (now - lastTxMs_ < 1) {
        return false;
    }
    lastTxMs_ = now;

    uint8_t tx[8] = { 0 };
    const uint16_t code = currentToCode(_current);
    const uint8_t offset = static_cast<uint8_t>(regInfo_.txSlot * 2);
    // 电流给定为 int16，按大端序填入本电机对应的数据段
    tx[offset] = static_cast<uint8_t>(code >> 8);
    tx[offset + 1] = static_cast<uint8_t>(code & 0xFF);

    return Can::instance().transmitData(regInfo_.hcan, static_cast<uint16_t>(txStdID()), tx, sizeof(tx))
           == HAL_OK;
}

uint32_t DJIMotor::txStdID()
{
    return regInfo_.id <= MOTOR_NUM_PER_FRAME ? TX_STDID_1_4 : TX_STDID_5_7;
}

uint16_t DJIMotor::currentToCode(float _current)
{
    constexpr float NUM_PER_AMP = CURRENT_CODE_MAX / CURRENT_MAX;
    if (!std::isfinite(_current)) {
        _current = 0.0f;
    }
    float code = std::clamp(_current, -CURRENT_MAX, CURRENT_MAX) * NUM_PER_AMP;
    int32_t v = static_cast<int32_t>(std::lround(code));
    v = v > 16384 ? 16384 : (v < -16384 ? -16384 : v);
    return static_cast<uint16_t>(static_cast<int16_t>(v));
}

hpp
#pragma once

#include "MotorMsgs.hpp"
#include "Bsp_can.hpp"

namespace MOTOR {

class DJIMotor {
public:
    //param

    static constexpr float CURRENT_MAX = 3.0f;
    static constexpr float CURRENT_CODE_MAX = 16384.0f;
    // 一帧数据可容纳的电机数量（每台电机占用 2 字节）
    static constexpr uint8_t MOTOR_NUM_PER_FRAME = 4;

    DJIMotor() = default;

    void init(canHandle *_hcan, uint8_t _id);

    bool cmdCurrent(float _txData);

private:
    // DJI 电调电流控制帧的标准帧 ID：ID 1~4 共用 0x200，ID 5~8 共用 0x1FF
    static constexpr uint32_t TX_STDID_1_4 = 0x200;
    static constexpr uint32_t TX_STDID_5_7 = 0x1FF;

    uint16_t currentToCode(float _current);
    uint32_t txStdID();

    uint32_t lastTxMs_ = 0;
    DJIRegInfo_s regInfo_ = {};
    bool inited_ = false;
};

} // namespace MOTOR
