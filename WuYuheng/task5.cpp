#include <iostream>
#include <iterator>
#ifdef _WIN32
#include <windows.h>
#endif
using namespace std;
int main() {
#ifdef _WIN32
    // 设置控制台按 UTF-8 解读输出,解决中文乱码(仅 Windows 生效,不影响 Linux)
    SetConsoleOutputCP(CP_UTF8);
#endif
    int Data[4] = {0x05, 0x6C, 0xB1, 0xBC};
    int id, vel, accel, temp, torque, voltage;
    id=Data[0];
    vel=Data[1]&0x0F;
    accel=(Data[1]>>4)&0x0F;
    temp=Data[2]&0x3F;
    voltage=(Data[3]>>4)&0x0F;
    torque=(((Data[3]>>6)&0x03)<<4)+(Data[3]&0x0F);
    cout << id <<endl<< vel <<endl<< accel <<endl<< temp <<endl<< voltage <<endl<<torque;
    return 0;
}