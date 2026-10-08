#include <iostream>
using namespace std;
#include <string>
#include <cstdint>
uint8_t Data[4] = {0x05,0x6C,0xB1,0xBC};
uint8_t id,vel,accel,temp,torque,voltage;
int main(){

    id=Data[0];
    vel=Data[1]&0x0F;
    accel=(Data[1]>>4)&0x0F;
    temp=Data[2]&0x3F;
    torque=((Data[2]>>6)<<4)|(Data[3]&0x0F);
    voltage=(Data[3]>>4)&0x0F;
    struct A{
        string name;
        int zhi;
    };
    A zu[6]={
        {"ID",id},
        {"速度",vel},
        {"加速度",accel},
        {"温度",temp},
        {"扭矩",torque},
        {"电压",voltage}
    } ;
    for(A* p=zu;p<zu+6;p++){
        cout<<p->name<<"="<<p->zhi<<endl;
    }
        
    return 0;
}