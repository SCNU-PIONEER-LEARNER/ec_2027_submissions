#include "GM6020.hpp"
#include "CAN.hpp"
#include "Projdefs.hpp"
#include "Soc.hpp"
#include <assert.h>
#include <cstdint>
#include <cstring>
#include <cmath>
#include "FreeRTOS.h"
#include "task.h"

extern CAN_HandleTypeDef HCAN1;
extern CAN_HandleTypeDef HCAN2;

GM6020::GM6020(InitConfig_s _config)
        : cmdType(_config.cmdType), txBaseId(_config.txBaseId), rxBaseId(_config.rxBaseId), offsetId(_config.offsetId)
{
    cmdClear();
}

GM6020::~GM6020()
{
    CAN::instance().setRxStdId(0x0000);
    cmdClear();
}
MotorData_s GM6020::getMotorData()
{
    memcpy(rxBuf, CAN::instance().getCAN1RxData(), 8);

    rawData.rawAng = ((rxBuf[1] << 8) | rxBuf[0]);
    rawData.rawVel = static_cast<int16_t>((rxBuf[3] << 8) | rxBuf[2]);
    rawData.rawCurr = static_cast<int16_t>((rxBuf[5] << 8) | rxBuf[4]);
    rawData.rawTemp = rxBuf[6];

    motorData.Ang = static_cast<float>(rawData.rawAng) / (ANG_MEASURE_MAX - ANG_MEASURE_MIN) * 2.f * M_PI;
    motorData.Vel = static_cast<float>(rawData.rawVel);
    motorData.current = static_cast<float>(rawData.rawCurr) / CURR_MEASURE_MAX * CURR_MAX;
    motorData.torq = motorData.current * KN;
    motorData.temperture = rawData.rawTemp;

    return motorData;
}

void GM6020::cmd(float _value)
{
    if (cmdType == CmdType_e::SET_CURR) {
        cmdElec.elec = _value;
        cmdElec.rawCmd = cmdElec.elec;
    }
    if (cmdType == CmdType_e::SET_TORQ) {
        cmdTorq.torq = _value;
        cmdTorq.elec = cmdTorq.torq / KN;
        cmdTorq.rawCmd = cmdTorq.elec;
    }
    if (cmdType == CmdType_e::SET_VOLT) {
        cmdVolt.volt = _value;
        cmdVolt.rawCmd = cmdVolt.volt;
    }
    sendCmdData();
}

void GM6020::disable()
{
    cmdType = CmdType_e::OFF;
    sendCmdData();
}

void GM6020::cmdClear()
{
    cmdElec.elec = 0.f;
    cmdTorq.torq = 0.f;
    cmdTorq.elec = 0.f;

    for (int i = 0; i < 8; i++) {
        txBuf[i] = 0;
    }
}

void GM6020::sendCmdData()
{
    if (cmdType == CmdType_e::SET_CURR) {
        txBuf[2 * (offsetId - 1)] = static_cast<uint8_t>(static_cast<int16_t>(cmdElec.rawCmd) >> 8 & 0xFF);
        txBuf[2 * (offsetId - 1) + 1] = static_cast<uint8_t>(static_cast<int16_t>(cmdElec.rawCmd) & 0xFF);
    }
    if (cmdType == CmdType_e::SET_TORQ) {
        txBuf[2 * (offsetId - 1)] = static_cast<uint8_t>((static_cast<int16_t>(cmdTorq.rawCmd) >> 8) & 0xFF);
        txBuf[2 * (offsetId - 1) + 1] = static_cast<uint8_t>((static_cast<int16_t>(cmdTorq.rawCmd)) & 0xFF);
    }
    if (cmdType == CmdType_e::SET_VOLT) {
        txBuf[2 * (offsetId - 1)] = static_cast<uint8_t>((static_cast<int16_t>(cmdVolt.rawCmd) >> 8) & 0xFF);
        txBuf[2 * (offsetId - 1) + 1] = static_cast<uint8_t>((static_cast<int16_t>(cmdVolt.rawCmd)) & 0xFF);
    }
    if (cmdType == CmdType_e::OFF) {
        txBuf[2 * (offsetId - 1) + 1] = 0 & 0xFF;
        txBuf[2 * (offsetId - 1)] = (0 >> 8) & 0xFF;
    }

    CAN::instance().transmitData(&HCAN2, txBaseId, txBuf, 8);
}
