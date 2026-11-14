#pragma once

#include <cstdint>
enum class CmdType_e : uint8_t { OFF, SET_TORQ, SET_CURR, SET_VOLT };


struct InitConfig_s{
    CmdType_e cmdType;

    uint16_t txBaseId;
    uint16_t rxBaseId;
    uint8_t offsetId;
};

struct CmdData_s{
    float torq;
    float elec;
    float volt;
    float rawCmd;    
};

struct MotorData_s{
    float Ang;
    float Vel;//角速度，单位为rpm
    float torq;
    float current;
    float temperture;

};

struct MotorRawData_s{
    uint16_t rawAng;
    int16_t rawVel;
    int16_t rawCurr;
    uint8_t rawTemp;
};