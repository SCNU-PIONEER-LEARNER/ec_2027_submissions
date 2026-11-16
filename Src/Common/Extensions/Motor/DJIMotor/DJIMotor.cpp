#include "DJIMotor.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>

using namespace MOTOR;

void DJIMotor::init(canHandle *_hcan, uint8_t _id)
{
    regInfo_ = {};//初始化regInfo_结构体为0，清空旧数据

    if (_id < 1 || _id > 7) {
        return;
    }

    regInfo_.hcan = _hcan;
    regInfo_.id = _id;

    if (_id <= 4) {
        regInfo_.txSlot = static_cast<uint8_t>(_id - 1);   // 1~4 → 0~3
    } else {
        regInfo_.txSlot = static_cast<uint8_t>(_id - 5);   // 5~7 → 0~2
    }//将8个字节的内容装进电机

}

bool DJIMotor::cmdCurrent(float _current)//换算，组帧，发送
{
    if (HAL_GetTick() - lastTxMs_ < 1)
        return false;

    lastTxMs_ = HAL_GetTick();//更新时间
    uint8_t tx[8] = { 0 };//初始化发送数据为0，让其他电机未接受数据时不受影响
    uint16_t code = currentToCode(_current);//将浮点电流换算成16进制值

    tx[regInfo_.txSlot * 2]     = code >> 8;
    tx[(regInfo_.txSlot * 2) + 1] = code & 0xFF;
    return Can::instance().transmitData(regInfo_.hcan, txStdID(), tx, 8) == HAL_OK;
}


uint16_t DJIMotor::currentToCode(float _current)//电流换算成电机的编码值
{
    constexpr float NUM_PER_AMP = CURRENT_CODE_MAX / CURRENT_MAX;
    float code = std::clamp(_current, -CURRENT_MAX, CURRENT_MAX) * NUM_PER_AMP;
    int32_t v = static_cast<int32_t>(std::lround(code));
    v = v > 16384 ? 16384 : (v < -16384 ? -16384 : v);
    return static_cast<uint16_t>(static_cast<int16_t>(v));
}
uint32_t DJIMotor::txStdID()
{
    return (regInfo_.id <= 4) ? TX_STDID_1_4 : TX_STDID_5_7;
}