#include <iostream>
#include <windows.h>

using namespace std;

// 从 data 里取出第 low 位到第 high 位（最右边是第 0 位）
// 做法：先右移 low 位，把要取的位挪到最右边，再用 & 掩码留下需要的几位
unsigned char extractBits(unsigned char data, int low, int high) {
    int width = high - low + 1;              // 要取几位
    unsigned char mask = (1 << width) - 1;   // 掩码：比如取 4 位就是 00001111
    return (data >> low) & mask;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);   // 让终端正常显示中文

    unsigned char Data[4] = {0x05, 0x6C, 0xB1, 0xBC};   // 题目给的 4 个字节数据

    unsigned char id    = extractBits(Data[0], 0, 7);   // ID 占整个字节
    unsigned char vel   = extractBits(Data[1], 0, 3);   // 速度占低 4 位
    unsigned char accel = extractBits(Data[1], 4, 7);   // 加速度占高 4 位
    unsigned char temp  = extractBits(Data[2], 0, 5);   // 温度占 6 位

    // 扭矩 6 位被拆在两个字节里：Data[2] 的高 2 位 + Data[3] 的低 4 位
    unsigned char torqueHi = extractBits(Data[2], 6, 7);
    unsigned char torqueLo = extractBits(Data[3], 0, 3);
    unsigned char torque   = (torqueHi << 4) | torqueLo;   // 高位左移后拼上低位

    unsigned char voltage = extractBits(Data[3], 4, 7);   // 电压占高 4 位

    // (int) 是把 unsigned char 转成整数打印，不然 cout 会当字符输出
    cout << "ID     = " << (int)id << endl;
    cout << "速度   = " << (int)vel << endl;
    cout << "加速度 = " << (int)accel << endl;
    cout << "温度   = " << (int)temp << endl;
    cout << "扭矩   = " << (int)torque << endl;
    cout << "电压   = " << (int)voltage << endl;

    system("pause");   // 按任意键再关窗口
    return 0;
}
