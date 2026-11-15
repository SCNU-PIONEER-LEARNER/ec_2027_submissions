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
    HAL_StatusTypeDef result = HAL_OK;
    result = initSelf(&HCAN1, RX_FIFO0);
    result = initSelf(&HCAN2, RX_FIFO1);
#ifdef HCAN3
    extern canHandle HCAN3;
    result = initSelf(&HCAN3, RX_FIFO0);
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
/* F4 的 28 组过滤器由 CAN1/CAN2 共用，靠 CAN_FMR.CAN2SB（复位默认 14）划界：
 *   bank 0~13  → 只对 CAN1 生效
 *   bank 14~27 → 只对 CAN2 生效
 * 所以给 CAN2 配 bank0 等于没配；两个外设必须各用各的 bank，否则后配的会覆盖前者。*/
static constexpr uint8_t FILTER_BANK_CAN1 = 0;
#if SOC_CAN_NUM >= 2
static constexpr uint8_t FILTER_BANK_CAN2 = 14;
#endif

HAL_StatusTypeDef canFilterInit(CAN_HandleTypeDef *_can, uint32_t _fifo)
{
    HAL_StatusTypeDef result = HAL_OK;
    CAN_FilterTypeDef canFilter{};           //接收过滤器
   canFilter.FilterBank           = FILTER_BANK_CAN1; //默认给 CAN1
#if SOC_CAN_NUM >= 2
   if (_can == &HCAN2) {
       canFilter.FilterBank       = FILTER_BANK_CAN2; //CAN2 只能用 14~27
   }
#endif
   canFilter.FilterMode           = CAN_FILTERMODE_IDMASK;
   canFilter.FilterScale          = CAN_FILTERSCALE_32BIT;
   canFilter.FilterIdHigh         = 0;   // 标准帧 ID 左移 5 位
   canFilter.FilterIdLow          = 0;
   canFilter.FilterMaskIdHigh     = 0 ; //ilterMaskIdHigh/Low	掩码模式下的掩码，位为 1 表示该位必须匹配
   canFilter.FilterMaskIdLow      = 0;
   canFilter.FilterFIFOAssignment = (_fifo == 0) ? CAN_FILTER_FIFO0 : CAN_FILTER_FIFO1; //命中的报文进 CAN_RX_FIFO0 
   canFilter.FilterActivation     = CAN_FILTER_ENABLE; //正式开启
    result = HAL_CAN_ConfigFilter(_can, &canFilter);
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
/*字段	作用
FilterBank	使用哪个过滤器组（F4 单 CAN 实例 0~13；双 CAN 共享 0~27）
FilterMode	CAN_FILTERMODE_IDMASK（掩码模式，一组可覆盖一个范围）或 IDLIST（列表模式，精确匹配若干 ID）
FilterScale	16BIT（一个 bank 拆成 2 个 16 位过滤器）或 32BIT（一个 bank 只放 1 个过滤器，可含扩展帧）
FilterIdHigh/Low	期望的 ID 值（32 位模式：High 存 STID<<5 | IDE<<2 | RTR，Low 存 EXID<<3）
FilterMaskIdHigh/Low	掩码模式下的掩码，位为 1 表示该位必须匹配；列表模式下则为第二个 ID
FilterFIFOAssignment	命中的报文进 CAN_RX_FIFO0 还是 FIFO1
FilterActivation	CAN_FILTER_ENABLE 才真正生效
SlaveStartFilterBank	双 CAN 时分配给从机(CAN2)的起始 bank，单 CAN 无意义
*/
/*_hcan	canHandle*（= CAN_HandleTypeDef*，Soc.hpp 里的宏别名）	用哪个 CAN 外设发，如 &HCAN1（宏 → hcan1）	直接传给 HAL_CAN_AddTxMessage(_hcan, ...)，HAL 靠它找寄存器基址和状态
_stdid	uint16_t	标准帧 ID（11 位，0~0x7FF）	写入 txHeader.StdId；电机那边传的是 0x1FE / 0x2FE
_txData	uint8_t*	待发数据的首地址，非 const（HAL 的接口就是非 const 指针）	传给 HAL_CAN_AddTxMessage(..., _txData, ...)，硬件按字节搬进邮箱
_len	uint32_t	数据长度，即 DLC，0~8 字节*/