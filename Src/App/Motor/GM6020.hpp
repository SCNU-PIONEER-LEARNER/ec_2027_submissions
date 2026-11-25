#pragma once
#include"Projdefs.hpp"
#include <cstdint>


class GM6020{
    static constexpr float ANG_MEASURE_MAX=8191.f;
    static constexpr float ANG_MEASURE_MIN=0.f;

    static constexpr float CURR_MEASURE_MAX=16384.f;// 量程：报文里的“格数”满量程
    static constexpr float CURR_MAX=3.f; // 物理：满量程对应 3 安培

    static constexpr float VOLT_MEASURE_MAX=25000.f;
    static constexpr float VOLT_MAX=25.2f;

    static constexpr float CURR_RATED=1.62f;//额定电流
    static constexpr float TORQ_RATED=1.2f;//额定扭矩
    static constexpr float KN = TORQ_RATED/CURR_RATED;

    public:
    GM6020(InitConfig_s _config);
    ~GM6020();
    bool isMotorOnline();
    
    
    void cmd(float _value);
    void disable();
    void setCmdType(CmdType_e _type){cmdType = _type;};
    CmdType_e getCmdType(){return cmdType;};
    
    MotorData_s getMotorData();

    private:
    void sendCmdData();
    void cmdClear();

    CmdType_e cmdType;
    uint16_t txBaseId;
    uint16_t rxBaseId;
    uint8_t offsetId;

    uint8_t rxBuf[8];
    MotorRawData_s rawData;
    MotorData_s motorData;

    CmdData_s cmdElec;
    CmdData_s cmdTorq;
    CmdData_s cmdVolt;

    uint8_t txBuf[8];
};