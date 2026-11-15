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
      if (regInfo_.id < 1U || regInfo_.id > 7U) {
        regInfo_.id = 1U;   /* ID 非法时退回 1，防止后面数组越界 */
    }
    lastTxMs_ = HAL_GetTick() - 1U;   /* 保证第一次调用不被限速拦掉 */
}

bool DJIMotor::cmdCurrent(float _current)
{
    if (HAL_GetTick() - lastTxMs_ < 1)
        return false;

    lastTxMs_ = HAL_GetTick();
    uint8_t tx[8] = { 0 };
    uint16_t code = currentToCode(_current);
    // TODO:
    const uint8_t offset = static_cast<uint8_t>((regInfo_.id - 1U) * 2U);
    tx[offset]     = static_cast<uint8_t>(code & 0xFFU);
    tx[offset + 1] = static_cast<uint8_t>((code >> 8) & 0xFFU);
    const uint32_t stdid = (regInfo_.id <= 4U) ? 0x1FE : 0x2FE;
    Can::instance().transmitData(regInfo_.hcan, stdid, tx, 8);
    //“把刚才装好的 8 个字节，从指定 CAN 口、贴上指定 ID 发出去
    return false;
}

uint16_t DJIMotor::currentToCode(float _current)
{
    constexpr float NUM_PER_AMP = CURRENT_CODE_MAX / CURRENT_MAX;
    float code = std::clamp(_current, -CURRENT_MAX, CURRENT_MAX) * NUM_PER_AMP;
    int32_t v = static_cast<int32_t>(std::lround(code));
    v = v > 16384 ? 16384 : (v < -16384 ? -16384 : v);
    return static_cast<uint16_t>(static_cast<int16_t>(v));
}