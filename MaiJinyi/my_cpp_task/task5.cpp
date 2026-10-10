#include <iostream>
#include <cstdint>
using namespace std;
int main(){
    uint8_t data[4]={0x05,0x6c,0xB1,0xBC};
    uint8_t id,vel,accel,temp,torque,voltage;
    id=data[0];
    vel=data[1]&0x0F;
    accel=data[1]>>4;
    temp=data[2]&0x3F;
    uint8_t a=data[2]>>6;
    uint8_t b=data[3]&0x0F;
    voltage=data[3]>>4;
    torque=(a<<4)|b;
    cout<<"id is:"<<(int)id<<endl;
    cout<<"vel is:"<<(int)vel<<", accel is:"<<(int)accel<<endl;
    cout<<"temp is:"<<(int)temp<<", torque is:"<<(int)torque<<", voltage is:"<<(int)voltage<<endl;
    return 0;
}