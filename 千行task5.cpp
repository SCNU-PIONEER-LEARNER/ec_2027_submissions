#include <iostream>
#include <cstdint>
using namespace std;
int main(){
	uint8_t Data[4]={0x05,0x6C,0xB1,0xBC};
    uint8_t id,vel,accel,temp,torque_high,torque_low,voltage;  
    id=Data[0];
    vel=Data[1]&0x0F;
    accel=(Data[1]>>4)&0x0F;
    temp=Data[2]&0x3f;
    torque_high=(Data[2]>>6)&0x03;
    torque_low=Data[3]&0x0F;
    uint16_t torque=(torque_high<<4)|torque_low;
    voltage=(Data[3]>>4)&0x0F;
    
    cout<<"id="<<(int)id<<endl;
    cout<<"vel="<<(int)vel<<endl;
    cout<<"accel="<<(int)accel<<endl;
    cout<<"temp="<<(int)temp<<endl;
    cout<<"torque="<<(int)torque<<endl;
    cout<<"voltage="<<(int)voltage<<endl;
    
    return 0;
    

}

