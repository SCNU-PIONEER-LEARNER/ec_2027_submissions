#include <iostream>
#include <cstdint>
using namespace std;
int main()
{
    uint8_t data[4]={0x05,0x6C,0xB1,0xBC};
    uint8_t id,vel,accel,temp,torque_h,torque_l,torque,voltage;
    id=data[0];
    accel=(data[1]>>4)&0x0F;
vel=data[1]&0x0F;
 torque_h=(data[2]>>6)&0x03;
    temp=data[2]&0x3F;
    torque_l=data[3]&0x0F;
voltage=(data[3]>>4)&0x0F;
    torque=(torque_h<<4)|torque_l;
    cout<<"id:"<<id<<endl;
    cout<<"accel:"<<accel<<endl;
cout<<"vel:"<<vel<<endl;
    cout<<"temp:"<<temp<<endl;
    cout<<"torque:"<<torque<<endl;
    cout<<"voltage:"<<voltage<<endl;
    return 0;
}
