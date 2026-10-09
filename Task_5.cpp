#include <iostream>
using namespace std;
int main() {
    unsigned char Data[4] = {0x05, 0x6C, 0xB1, 0xBC};
    unsigned char id = Data[0];
    unsigned char vel = Data[1] & 0x0F;
    unsigned char accel = (Data[1] >> 4) & 0x0F;
    unsigned char temp = Data[2] & 0x3F;
    unsigned char torque_high = (Data[2] >> 6) & 0x03;
    unsigned char torque_low = Data[3] & 0x0F;
    unsigned char voltage = (Data[3] >> 4) & 0x0F;
    unsigned char torque = (torque_high << 4) | torque_low;   
    cout << "ID: " << (int)id << ", v: " << (int)vel << ", a: " << (int)accel << endl;
    cout << "Temperature: " << (int)temp << ", Torque: " << (int)torque << ", U: " << (int)voltage << endl;
    return 0;
}
