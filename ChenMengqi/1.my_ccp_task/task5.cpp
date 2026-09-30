#include<iostream>
#include <cstdint>
using namespace std;
int main()
{
uint8_t Data[4]={0x05, 0x6C, 0xB1, 0xBC};
uint8_t id,vel,accel,temp,torque,voltage;
id=Data[0];
vel=(Data[1]&0x0F);
accel=(Data[1]&0xF0)>>4;
temp=Data[2]&0x3F;
torque=((Data[2]&0xC0)>>6)|(Data[3]&0x0F);
voltage=(Data[3]&0xF0)>>4;
cout<<"id:"<<(int)id<<endl<<"vel:"<<(int)vel<<endl<<"accel:"<<(int)accel<<endl<<"temp:"<<(int)temp<<endl<<"torque:"<<(int)torque<<endl<<"voltage:"<<(int)voltage<<endl;
return 0;
}