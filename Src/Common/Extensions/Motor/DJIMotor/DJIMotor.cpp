#include "DJIMotor.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>

using namespace MOTOR;

void DJIMotor::init(canHandle *_hcan, uint8_t _id)
{
    if (_hcan == nullptr || _id < 1 || _id > 7) 
    {
        return;
    }
    regInfo_.hcan = _hcan;
    regInfo_.id = _id;
    regInfo_.txSlot = (_id <= 4) ? static_cast<uint8_t>(_id - 1) : static_cast<uint8_t>(_id - 5);
    // TODO:can回调
    // GM6020 反馈帧固定为 0x204 + 电调ID(0x205~0x20B)
    Can::instance().registerCallback(
            regInfo_.hcan, 0x204U + regInfo_.id,
            [this](const uint8_t *_data) 
            {
                rxAngle_ = static_cast<uint16_t>((_data[0] << 8) | _data[1]);
                rxRpm_ = static_cast<int16_t>((_data[2] << 8) | _data[3]);
                rxCurrent_ = static_cast<int16_t>((_data[4] << 8) | _data[5]);
                rxTemperature_ = _data[6];
                lastRxMs_ = HAL_GetTick();
            },
            8);
}

bool DJIMotor::cmdCurrent(float _current)
{
    if (regInfo_.hcan == nullptr)
        return false;
    if (HAL_GetTick() - lastTxMs_ < 1)
        return false;

    lastTxMs_ = HAL_GetTick();
    uint8_t tx[8] = { 0 };
    uint16_t code = currentToCode(_current);
    // TODO:
    const uint8_t byteOffset = static_cast<uint8_t>(regInfo_.txSlot * 2U);
    tx[byteOffset] = static_cast<uint8_t>(code >> 8);
    tx[byteOffset + 1] = static_cast<uint8_t>(code & 0x00FFU);
    return Can::instance().transmitData(regInfo_.hcan, static_cast<uint16_t>(txStdID()), tx, sizeof(tx)) == HAL_OK;
}

uint16_t DJIMotor::currentToCode(float _current)  //-3~3  0.8
{
    constexpr float NUM_PER_AMP = CURRENT_CODE_MAX / CURRENT_MAX;
    float code = std::clamp(_current, -CURRENT_MAX, CURRENT_MAX) * NUM_PER_AMP;
    int32_t v = static_cast<int32_t>(std::lround(code));
    v = v > 16384 ? 16384 : (v < -16384 ? -16384 : v);
    return static_cast<uint16_t>(static_cast<int16_t>(v));
}

uint32_t DJIMotor::txStdID()
{
    return regInfo_.id <= 4 ? TX_STDID_1_4 : TX_STDID_5_7;
}
