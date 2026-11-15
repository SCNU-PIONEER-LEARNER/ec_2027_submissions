#pragma once

#include <cstdint>
#include "Bsp_can.hpp"

namespace MOTOR {

struct DJIRegInfo_s {
    canHandle *hcan;   //该电机所在的 CAN 外设句柄
    uint8_t id;        
    uint8_t txSlot; // 0~6，在该控制帧 8 字节里的槽位
};

} // namespace MOTOR
