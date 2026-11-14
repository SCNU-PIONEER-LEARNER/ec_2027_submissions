#pragma once
#include "Singleton.hpp"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_can.h"
#include "stm32f4xx_hal_def.h"
#include <cstdint>

class CAN : public Singleton<CAN> {
public:
    CAN();
    HAL_StatusTypeDef init();
    void callbackFromISR(CAN_HandleTypeDef *_hcan, uint32_t _rxFifo);
    HAL_StatusTypeDef transmitData(CAN_HandleTypeDef *_hcan, uint16_t _stdid, uint8_t *_txData, uint32_t _len);

    void setRxStdId(uint32_t _id) { rxStdId = _id; }

    using rxDataArray = const uint8_t[8];
    const rxDataArray &getCAN1RxData() { return rxBuffer1; }
    const float &getRxFreq() { return rxFreq; }

private:
    friend class Singleton<CAN>;

    HAL_StatusTypeDef canFilterInit(CAN_HandleTypeDef *_can, uint32_t _fifo);

    uint8_t rxBuffer0[8];
    uint8_t rxBuffer1[8];
    uint8_t rxData[8];
    uint32_t rxStdId;

    uint16_t rxCnt;
    uint32_t lastRxTick;
    float rxFreq;
};