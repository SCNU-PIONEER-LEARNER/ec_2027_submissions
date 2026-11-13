#include "Bsp_can.hpp"
#include "Soc.hpp"
#include "stm32f4xx_hal_can.h"

extern canHandle HCAN1;
extern canHandle HCAN2;
#if SOC_CAN_NUM == 3
extern canHandle HCAN3;
#endif

HAL_StatusTypeDef Can::init()
{
    /* 原来是 result = initSelf(...) 连写三行，后一行的返回值会把前一行的覆盖掉，
     * CAN1 初始化失败会被静默吞掉。这里改成任一失败就置位，不丢失错误。 */
    HAL_StatusTypeDef result = HAL_OK;
    if (initSelf(&HCAN1, RX_FIFO0) != HAL_OK) {
        result = HAL_ERROR;
    }
    if (initSelf(&HCAN2, RX_FIFO1) != HAL_OK) {
        result = HAL_ERROR;
    }
#ifdef HCAN3
    extern canHandle HCAN3;
    if (initSelf(&HCAN3, RX_FIFO0) != HAL_OK) {
        result = HAL_ERROR;
    }
#endif
    return result;
}


HAL_StatusTypeDef Can::registerCallback(canHandle *_hcan, uint32_t _stdid, callback _pCallback,
                                       uint8_t _expectedLength)
{
    if (_expectedLength > 8 || !_pCallback) {
        return HAL_ERROR;
    }
    struct Handler_s temp = { .stdid = _stdid, .func = _pCallback, .expectedLength = _expectedLength };
    if (_hcan == &HCAN1) {
        if (can1cnt < MAX_RECV_DEVICE) {
            cbTable1[can1cnt] = temp;
            can1cnt++;
            return HAL_OK;
        } else {
            return HAL_ERROR;
        }
    } else if (_hcan == &HCAN2) {
        if (can2cnt < MAX_RECV_DEVICE) {
            cbTable2[can2cnt] = temp;
            can2cnt++;
            return HAL_OK;
        } else {
            return HAL_ERROR;
        }
    }
    return HAL_ERROR;
}

void Can::unregisterCallback(canHandle *_hcan, uint32_t _stdid)
{
    if (_hcan == &HCAN1) {
        for (uint8_t i = 0; i < can1cnt; i++) {
            if (cbTable1[i].stdid == _stdid) {
                cbTable1[i].func = nullptr;
                break;
            }
        }
    } else if (_hcan == &HCAN2) {
        for (uint8_t i = 0; i < can2cnt; i++) {
            if (cbTable2[i].stdid == _stdid) {
                cbTable2[i].func = nullptr;
                break;
            }
        }
    }
}

#if defined(SOC_CAN)
HAL_StatusTypeDef canFilterInit(CAN_HandleTypeDef *_can, uint32_t _fifo)
{
    HAL_StatusTypeDef result = HAL_OK;
    CAN_FilterTypeDef canFilter{};

    /* ★ 本题填写开始（原骨架为 // TODO:）
     * 全零结构体 = FilterActivation DISABLE = 一组滤波器都没使能
     * → STM32F4 收到帧后无处可放，直接丢弃，一帧都进不来。
     *   现象：CAN 状态正常、发送能成功，但接收回调永远不被调用。 */
    canFilter.FilterActivation = ENABLE;                 // 使能这组滤波器
    canFilter.FilterMode       = CAN_FILTERMODE_IDMASK;  // 掩码模式（逐位比对）
    canFilter.FilterScale      = CAN_FILTERSCALE_32BIT;  // 32 位宽：一个组同时放 ID 和掩码
    canFilter.FilterIdHigh     = 0x0000;                 // 期望 ID = 0
    canFilter.FilterIdLow      = 0x0000;
    canFilter.FilterMaskIdHigh = 0x0000;                 // 掩码全 0 = 不关心任何位 = 全通
    canFilter.FilterMaskIdLow  = 0x0000;

    /* 帧进了哪个 FIFO，必须和上面开的中断一致（RX_FIFO0=0 / RX_FIFO1=1） */
    canFilter.FilterFIFOAssignment = (_fifo == 0) ? CAN_RX_FIFO0 : CAN_RX_FIFO1;

    /* F407 共 28 组滤波器，CAN1 用 0~13，CAN2 用 14~27。
     * 注意 HAL 里这个编号是"绝对编号"，CAN2 传 0 会去改 CAN1 的 0 号组。 */
    canFilter.FilterBank           = (_can == &HCAN1) ? 0 : 14;
    canFilter.SlaveStartFilterBank = 14;
    /* ★ 本题填写结束 */

    result = HAL_CAN_ConfigFilter(_can, &canFilter);
    if (result != HAL_OK) {
        return result;
    }
    if (_fifo == 0) {
        result = HAL_CAN_ActivateNotification(_can, CAN_IT_RX_FIFO0_MSG_PENDING);
    } else if (_fifo == 1) {
        result = HAL_CAN_ActivateNotification(_can, CAN_IT_RX_FIFO1_MSG_PENDING);
    }
    return result;
}

HAL_StatusTypeDef Can::initSelf(canHandle *_hcan, uint32_t _fifo)
{
    HAL_StatusTypeDef result = HAL_OK;
    result = canFilterInit(_hcan, _fifo);
    __HAL_CAN_ENABLE_IT(_hcan, CAN_IT_BUSOFF);
    result = HAL_CAN_Start(_hcan);
    return result;
}

HAL_StatusTypeDef Can::transmitData(canHandle *_hcan, uint16_t _stdid, uint8_t *_txData, uint32_t _len)
{
    CAN_TxHeaderTypeDef txHeader;
    uint32_t canMailbox;
    txHeader.DLC = _len;
    txHeader.IDE = CAN_ID_STD;
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.StdId = _stdid;
    txHeader.TransmitGlobalTime = DISABLE;
    return HAL_CAN_AddTxMessage(_hcan, &txHeader, _txData, &canMailbox);
}

void Can::callbackFromISR(canHandle *_hcan, uint32_t _rxFifo)
{
    uint8_t rxData[8];
    canHeader rxHeader;
    if (HAL_CAN_GetRxMessage(_hcan, _rxFifo, &rxHeader, rxData) != HAL_OK) {
        return;
    }
    if (rxHeader.IDE != CAN_ID_STD || rxHeader.RTR != CAN_RTR_DATA) {
        return;
    }
    if (_hcan == &HCAN1) {
        for (uint8_t i = 0; i < can1cnt; i++) {
            if (cbTable1[i].stdid == rxHeader.StdId) {
                const auto &handler = cbTable1[i];
                if (handler.func && (handler.expectedLength == 0 || rxHeader.DLC == handler.expectedLength)) {
                    handler.func(rxData);
                }
                break;
            }
        }
    } else if (_hcan == &HCAN2) {
        for (uint8_t i = 0; i < can2cnt; i++) {
            if (cbTable2[i].stdid == rxHeader.StdId) {
                const auto &handler = cbTable2[i];
                if (handler.func && (handler.expectedLength == 0 || rxHeader.DLC == handler.expectedLength)) {
                    handler.func(rxData);
                }
                break;
            }
        }
    }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *_hcan) { Can::instance().callbackFromISR(_hcan, RX_FIFO0); }

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *_hcan) { Can::instance().callbackFromISR(_hcan, RX_FIFO1); }

#endif
