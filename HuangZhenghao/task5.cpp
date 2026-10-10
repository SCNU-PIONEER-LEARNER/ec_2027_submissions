#include <stdio.h>
#include <stdint.h>

int main() {
    uint8_t Data[4] = {0x05, 0x6C, 0xB1, 0xBC};
    uint8_t id, vel, accel, temp, torque_high,torque_low,voltage;
    id=Data[0];
    vel=Data[1]&0x0F;
    accel=(Data[1]>>4)&0x0F;
    temp=Data[2]&0x3F;
    torque_high=(Data[2]>>6)&0x03;
    torque_low=Data[3]&0x0F;
    voltage=(Data[3]>>4)&0x0F;
    printf("ID为:%d\n",id);
    printf("速度为:%d\n",vel);
    printf("加速度为:%d\n",accel);
    printf("温度为:%d\n",temp);
    printf("扭矩（高位）为:%d\n",torque_high);
    printf("扭矩(低位)为:%d\n",torque_low);
    printf("电压为:%d\n",voltage);
    
    return 0;
}