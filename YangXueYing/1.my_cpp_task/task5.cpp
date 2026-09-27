#include <iostream>
#include <cstdint>
using namespace std;

int main(){
    uint8_t Data[4] = {0x05, 0x6C, 0xB1, 0xBC};

    uint8_t id, vel, accel, temp, torque, voltage, torque_h, torque_l;

    id = Data[0];
    vel = Data[1] & 0x0F;
    accel = (Data[1] >> 4) & 0x0F;
    temp = Data[2] & 0x3F;
    torque_h = (Data[2] >> 6) & 0x03;
    torque_l = Data[3] & 0x0F;
    voltage = (Data[3] >> 4) & 0x0F;
    torque = (torque_h << 4) | torque_l;

    uint8_t vals[8] = {id, vel, accel, temp, torque_h, torque_l, torque, voltage};
    int bits[8]     = {8,  4,   4,     6,    2,           4,          6,      4};
    string names[8] = {"id", "vel", "accel", "temp", "torque_h", "torque_l", "torque", "voltage"};

    for (int k = 0; k < 8; k++) {
        cout << names[k] << " = ";
        for (int i = bits[k] - 1; i >= 0; i--) {
            cout << ((vals[k] >> i) & 1);
        }
        cout << endl;
    }

    return 0;
}



