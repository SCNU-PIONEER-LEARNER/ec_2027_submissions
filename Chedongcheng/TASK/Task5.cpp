#include <iostream>
using namespace std;
#include <cstdint>
int main() {
    system("chcp 65001");
    uint8_t Date[4]= {0x05, 0x6C, 0xB1, 0xBC};
    uint8_t id, vel, accel, temp, torque, voltage;
    id=Date[0];
    vel=Date[1]& 0x0F;
    accel=(Date[1]&0xF0)>>4;
    temp=Date[2]&0x3F;
    torque=((Date[2] & 0xC0) >> 6) << 4 | (Date[3] & 0x0F);
    voltage=(Date[3]&0xF0);
    cout << "ID= " <<(int) id << endl;
    cout << "速度= " <<(int) vel << endl;
    cout << "加速度= " <<(int) accel << endl;
    cout << "温度= " <<(int) temp << endl;
    cout << "扭矩= " <<(int) torque << endl;
    cout << "电压= " <<(int) voltage << endl;


    return 0;}