#include "DJIMotor.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>

using namespace MOTOR;

void DJIMotor::init(canHandle *_hcan, uint8_t _id)
{
    regInfo_.hcan = _hcan;
    regInfo_.id = _id;
    // TODO:
    regInfo_.txSlot = static_cast<uint8_t>((_id - 1) % 4);
}

bool DJIMotor::cmdCurrent(float _current)
{
    if (HAL_GetTick() - lastTxMs_ < 1)
        return false;

    lastTxMs_ = HAL_GetTick();
    uint8_t tx[8] = { 0 };
    uint16_t code = currentToCode(_current);
    // TODO: 数据打包与发送
    // 计算当前电机数据在 8 字节数组中的起始索引
    int idx = (regInfo_.id - 1) % 4 * 2;
    
    // 大端模式拆包：高 8 位在前，低 8 位在后
    tx[idx]     = (code >> 8) & 0xFF; 
    tx[idx + 1] = code & 0xFF;        

   return Can::instance().transmitData(regInfo_.hcan, txStdID(), tx, 8) == HAL_OK;
}

uint16_t DJIMotor::currentToCode(float _current)//电流范围-3--3
{
    constexpr float NUM_PER_AMP = CURRENT_CODE_MAX / CURRENT_MAX;
    float code = std::clamp(_current, -CURRENT_MAX, CURRENT_MAX) * NUM_PER_AMP;
    int32_t v = static_cast<int32_t>(std::lround(code));
    v = v > 16384 ? 16384 : (v < -16384 ? -16384 : v);
    return static_cast<uint16_t>(static_cast<int16_t>(v));
}
uint32_t DJIMotor::txStdID() {
    if (regInfo_.id >= 1 && regInfo_.id <= 4) {
        return TX_STDID_1_4; // 返回 0x1FF
    } else if (regInfo_.id >= 5 && regInfo_.id <= 7) {
        return TX_STDID_5_7; // 返回 0x2FF
    }
    return 0; // 没找到对应的 ID 返回 0（或者 0x1FF 兜底）
}