#include"iostream"
using namespace std;

int main(){
    int Data[4] = {0x05, 0x6C, 0xB1, 0xBC};
    int id, vel, accel, temp, torque, voltage;
    id=Data[0];
    vel=Data[1]&0b00001111;
    accel=(Data[1]&0b11110000)>>4;
    temp=Data[2]&0b00111111;
    
    torque=((Data[2]&0b11000000)>>2)|(Data[3]&0b00001111);

    voltage=(Data[3]&0b11110000)>>4;

    cout<<id<<endl<<vel<<endl<<accel<<endl<<temp<<endl<<torque<<endl<<voltage<<endl;
    return 0;
}