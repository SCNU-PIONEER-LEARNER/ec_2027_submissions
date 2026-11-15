#include "DJIMotor.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>

using namespace MOTOR;
//电机对象自身的初始化
void DJIMotor::init(canHandle *_hcan, uint8_t _id) 
{
    regInfo_.hcan = _hcan;
    regInfo_.id = _id;
    // TODO:
    regInfo_.txSlot = (_id - 1) % 4;//算出这台电机在控制帧里的位置
    lastTxMs_ = 0;
   
}
//DJI 电调（C620/C610 那套 CAN 协议）的下发电流指令函数
bool DJIMotor::cmdCurrent(float _current)
{
    if (HAL_GetTick() - lastTxMs_ < 1) //获取数据的限速
        return false;

    lastTxMs_ = HAL_GetTick();
    uint8_t tx[8] = { 0 }; //DLC: 8 字节
    uint16_t code = currentToCode(_current) ;
    // TODO:
    const uint8_t byte =regInfo_.txSlot * 2;      // 每台电机占 2 字节
    tx[byte] = code >> 8;  //DATA[0] 高 8 位
    tx[byte+1] = code & 0xFF;  //DATA[1] 低 8 位
    
    return Can::instance().transmitData(regInfo_.hcan, txStdID(), tx, 8); //顺利把帧交出去了
}
uint32_t DJIMotor::txStdID() //根据电机 ID 选出该用哪个 CAN 标准帧 ID 来下发电流。
{
    if (regInfo_.id >= 1 && regInfo_.id <= 4) {
        return TX_STDID_1_4;   // 0x1FE
    }
    if (regInfo_.id >= 5 && regInfo_.id <= 7) {
        return TX_STDID_5_7;   // 0x2FE
    }
    return 0;
}


//把「物理电流（安培）」换算成「DJI 电调能认的 16 位整型码

uint16_t DJIMotor::currentToCode(float _current)
{
    constexpr float NUM_PER_AMP = CURRENT_CODE_MAX / CURRENT_MAX;
    float code = std::clamp(_current, -CURRENT_MAX, CURRENT_MAX) * NUM_PER_AMP;
    int32_t v = static_cast<int32_t>(std::lround(code));
    v = v > 16384 ? 16384 : (v < -16384 ? -16384 : v); //限幅
    return static_cast<uint16_t>(static_cast<int16_t>(v));
}