#include <iostream>
#include <cstdint>
using namespace std;

void printBin(uint8_t b) {
    for (int i = 7; i >= 0; i--) {
        cout << ((b >> i) & 1);
    }
}

int main() {
    uint8_t Data[4] = {0x05, 0x6C, 0xB1, 0xBC};

    uint8_t id = Data[0];

    uint8_t vel = Data[1] & 0x0F;

    uint8_t accel = Data[1] >> 4;

    uint8_t temp = Data[2] & 0x3F;

    uint8_t torque = ((Data[2] >> 6) << 4) | (Data[3] & 0x0F);

    uint8_t voltage = Data[3] >> 4;

    cout << "ID      = "; printBin(id);      cout << endl;
    cout << "速度    = "; printBin(vel);     cout << endl;
    cout << "加速度  = "; printBin(accel);   cout << endl;
    cout << "温度    = "; printBin(temp);    cout << endl;
    cout << "扭矩    = "; printBin(torque);  cout << endl;
    cout << "电压    = "; printBin(voltage); cout << endl;

    return 0;
}
