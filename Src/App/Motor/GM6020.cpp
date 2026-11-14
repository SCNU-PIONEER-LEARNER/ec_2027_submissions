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
     CAN::instance().setRxStdId(static_cast<int32_t>(rxBaseId +offsetId));
     //instance()是单例，
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

    rawData.rawAng = ((rxBuf[0] << 8) | rxBuf[1]); //接收缓冲
    rawData.rawVel = static_cast<int16_t>((rxBuf[2] << 8) | rxBuf[3]);
    rawData.rawCurr = static_cast<int16_t>((rxBuf[4] << 8) | rxBuf[5]);
    rawData.rawTemp = rxBuf[6];

    motorData.Ang = static_cast<float>(rawData.rawAng) / (ANG_MEASURE_MAX - ANG_MEASURE_MIN) * 2.f * M_PI;
    motorData.Vel = static_cast<float>(rawData.rawVel);
    motorData.current = static_cast<float>(rawData.rawCurr) / CURR_MEASURE_MAX * CURR_MAX;
    motorData.torq = motorData.current * KN;
    motorData.temperture = rawData.rawTemp;

    return motorData;
}

void GM6020::cmd(float _value)//cmd() 一次 = 发一帧 CAN
{
    if (cmdType == CmdType_e::SET_CURR) {
        cmdElec.elec = _value;//存值，记下想要的电流
        cmdElec.rawCmd = cmdElec.elec* (CURR_MEASURE_MAX / CURR_MAX);
    }
    if (cmdType == CmdType_e::SET_TORQ) {
        cmdTorq.torq = _value;
        cmdTorq.elec = cmdTorq.torq / KN;
        cmdTorq.rawCmd = cmdTorq.elec*(ANG_MEASURE_MAX / ANG_MEASURE_MIN);
    }
    if (cmdType == CmdType_e::SET_VOLT) {
        cmdVolt.volt = _value;
        cmdVolt.rawCmd = cmdVolt.volt* (VOLT_MEASURE_MAX / VOLT_MAX);// V → 格，≈992 格/V
    }
    sendCmdData();//拼字节，发Can
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
        txBuf[2 * (offsetId - 1)] = (0 >> 8) & 0xFF;//把一个 16 位大数劈两半装进两个字节，CAN 一帧只有 8 字节，装不下一个 16 位数
    }

    CAN::instance().transmitData(&hcan1, txBaseId, txBuf, 8);//发给"1 号到 4 号电机"的广播 ID
}
