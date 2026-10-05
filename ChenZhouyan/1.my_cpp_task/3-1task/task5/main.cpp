#include <iostream>
#include <bitset>
#include <windows.h>

using namespace std;

// 从一个字节数据里取出第 low 位到第 high 位（最右边是第 0 位）
// 思路：先往右移 low 位，把要取的位顶到最右边，再用 & 掩码留下需要的位数
unsigned char extractBits(unsigned char data, int low, int high) {
    int width = high - low + 1;       // 要取几位
    unsigned char mask = (1 << width) - 1;   // 掩码：取 4 位就是 0b1111
    return (data >> low) & mask;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);   // 让终端正常显示中文

    unsigned char Data[4] = {0x05, 0x6C, 0xB1, 0xBC};
    unsigned char id, vel, accel, temp, torque, voltage;

    id    = extractBits(Data[0], 0, 7);            // ID 占整个字节
    vel   = extractBits(Data[1], 0, 3);            // 速度低 4 位
    accel = extractBits(Data[1], 4, 7);            // 加速度高 4 位
    temp  = extractBits(Data[2], 0, 5);            // 温度 6 位

    // 扭矩 6 位被拆在两个字节里：Data[2] 的高 2 位 + Data[3] 的低 4 位
    unsigned char torqueHi = extractBits(Data[2], 6, 7);
    unsigned char torqueLo = extractBits(Data[3], 0, 3);
    torque = (torqueHi << 4) | torqueLo;           // 高位左移后拼上低位

    voltage = extractBits(Data[3], 4, 7);          // 电压 4 位

    // bitset<8> 可以把一个字节按 8 位二进制打出来
    cout << "ID     = " << (int)id      << "  | " << bitset<8>(id)      << endl;
    cout << "速度   = " << (int)vel     << "  | " << bitset<8>(vel)     << endl;
    cout << "加速度 = " << (int)accel   << "  | " << bitset<8>(accel)   << endl;
    cout << "温度   = " << (int)temp    << "  | " << bitset<8>(temp)    << endl;
    cout << "扭矩   = " << (int)torque  << "  | " << bitset<8>(torque)  << endl;
    cout << "电压   = " << (int)voltage << "  | " << bitset<8>(voltage) << endl;

    system("pause");   // 按任意键再关窗口
    return 0;
}
