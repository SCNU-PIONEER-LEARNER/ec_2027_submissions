#include "CAN.hpp"
#include "Common/Soc/stm32f103c8tx/Soc.hpp"
#include "Soc.hpp"
#include "stm32f4xx_hal_can.h"
#include "stm32f4xx_hal_def.h"
#include <cstring>
#include "FreeRTOS.h"
#include "task.h"

extern CAN_HandleTypeDef HCAN1;
extern CAN_HandleTypeDef HCAN2;

CAN::CAN() : rxStdId(0x0000), rxCnt(0)
{
    memset(rxData, 0, sizeof(rxData));
    memset(rxBuffer0, 0, sizeof(rxBuffer0));
    memset(rxBuffer1, 0, sizeof(rxBuffer1));
}

HAL_StatusTypeDef CAN::canFilterInit(CAN_HandleTypeDef *_can, uint32_t _fifo)
{
    HAL_StatusTypeDef result = HAL_OK;
    CAN_FilterTypeDef canFilter;
    canFilter.FilterActivation = ENABLE;           //使能过滤器
    canFilter.FilterMode = CAN_FILTERMODE_IDMASK;  //使用掩码
    canFilter.FilterScale = CAN_FILTERSCALE_32BIT; //过滤器位宽32位
    canFilter.FilterFIFOAssignment = _fifo;        //分配FIFO
    canFilter.FilterIdHigh = 0x0003;               //期望通过的ID高16位
    canFilter.FilterIdLow = 0x0003;                //期望通过的ID低16位
    canFilter.FilterMaskIdHigh = 0x0003;           //掩码高16位
    canFilter.FilterMaskIdLow = 0x0003;            //掩码低16位

    if (_can == &HCAN1) {
        canFilter.FilterBank = 0; //分配过滤器组0到14
    } else if (_can == &HCAN2) {
        canFilter.SlaveStartFilterBank = 14;
        canFilter.FilterBank = 14; //分配过滤器组15到28
    }
    result = HAL_CAN_ConfigFilter(_can, &canFilter);
    if (_fifo == 0) {
        result = HAL_CAN_ActivateNotification(_can, CAN_IT_RX_FIFO0_MSG_PENDING);
    } else if (_fifo == 1) {
        result = HAL_CAN_ActivateNotification(_can, CAN_IT_RX_FIFO1_MSG_PENDING);
    }

}

HAL_StatusTypeDef CAN::init()
{
    HAL_StatusTypeDef result = HAL_OK;

    result = canFilterInit(&HCAN1, 0);
    __HAL_CAN_ENABLE_IT(&HCAN1, CAN_IT_BUSOFF);
    result = HAL_CAN_Start(&HCAN1);

    result = canFilterInit(&HCAN2, 1);
    __HAL_CAN_ENABLE_IT(&HCAN2, CAN_IT_BUSOFF);
    result = HAL_CAN_Start(&HCAN2);

    return result;
}

void CAN::callbackFromISR(CAN_HandleTypeDef *_hcan, uint32_t _rxFifo)
{
    canHeader rxHeader;
    if (HAL_CAN_GetRxMessage(_hcan, _rxFifo, &rxHeader, rxData) != HAL_OK) {
        return;
    }

    if (_hcan == &HCAN1 && rxHeader.StdId == rxStdId) {
        memcpy(rxBuffer0, rxData, sizeof(rxData));
    } else if (_hcan == &HCAN2 && rxHeader.StdId == rxStdId) {
        memcpy(rxBuffer1, rxData, sizeof(rxData));
    }

}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *_hcan)
{
    CAN::instance().callbackFromISR(_hcan, CAN_RX_FIFO0);
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *_hcan)
{
    CAN::instance().callbackFromISR(_hcan, CAN_RX_FIFO1);
}

HAL_StatusTypeDef CAN::transmitData(CAN_HandleTypeDef *_hcan, uint16_t _stdid, uint8_t *_txData, uint32_t _len)
{
    CAN_TxHeaderTypeDef txHeader;
    uint32_t canMailbox;
    txHeader.DLC = _len;         //数据长度
    txHeader.IDE = CAN_ID_STD;   //标准帧
    txHeader.RTR = CAN_RTR_DATA; //数据帧
    txHeader.StdId = _stdid;     //标识符
    txHeader.TransmitGlobalTime = DISABLE;

    return HAL_CAN_AddTxMessage(_hcan, &txHeader, _txData, &canMailbox);
}