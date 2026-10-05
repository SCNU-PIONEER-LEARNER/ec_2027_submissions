#include <iostream>
#include <cstdint>
#include <bitset>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#endif

// 提取 data 的第 low 位到第 high 位（从右往左数，第 0 位是最低位）
// 做法：先右移 low 位，把目标位移到最右边；再用掩码保留 (high-low+1) 位
uint8_t extractBits(uint8_t data, int low, int high) {
    int width = high - low + 1;        // 要取几位
    uint8_t mask = (1 << width) - 1;   // 掩码：取 4 位就是 0b1111，取 6 位就是 0b111111
    return (data >> low) & mask;
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);  // 让 Windows 终端正确显示中文
#endif
    uint8_t Data[4] = {0x05, 0x6C, 0xB1, 0xBC};
    uint8_t id, vel, accel, temp, torque, voltage;

    id      = extractBits(Data[0], 0, 7);          // ID[0,7]：整 8 位
    vel     = extractBits(Data[1], 0, 3);          // 速度[0,3]
    accel   = extractBits(Data[1], 4, 7);          // 加速度[4,7]
    temp    = extractBits(Data[2], 0, 5);          // 温度[0,5]

    uint8_t torqueHi = extractBits(Data[2], 6, 7); // 扭矩高位[6,7]
    uint8_t torqueLo = extractBits(Data[3], 0, 3); // 扭矩低位[0,3]
    torque = (torqueHi << 4) | torqueLo;           // 高位拼低位，组成完整扭矩

    voltage = extractBits(Data[3], 4, 7);          // 电压[4,7]

    std::cout << "Data[4] = {0x05, 0x6C, 0xB1, 0xBC}\n\n";
    std::cout << "提取结果（十进制 | 8位二进制显示，高位补0）:\n";
    std::cout << "ID     = " << (int)id      << "  | " << std::bitset<8>(id)      << "\n";
    std::cout << "速度   = " << (int)vel     << "  | " << std::bitset<8>(vel)     << "\n";
    std::cout << "加速度 = " << (int)accel   << "  | " << std::bitset<8>(accel)   << "\n";
    std::cout << "温度   = " << (int)temp    << "  | " << std::bitset<8>(temp)    << "\n";
    std::cout << "扭矩   = " << (int)torque  << "  | " << std::bitset<8>(torque)  << "\n";
    std::cout << "电压   = " << (int)voltage << "  | " << std::bitset<8>(voltage) << "\n";

    system("pause");
    return 0;
}
