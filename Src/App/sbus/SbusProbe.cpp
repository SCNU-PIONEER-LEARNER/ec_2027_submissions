#include "SbusProbe.hpp"

#include "Soc.hpp"
#include HAL_INCLUDE
#include "StmLog.hpp"

extern "C" {
extern UART_HandleTypeDef huart3;
}

namespace {

constexpr uint16_t RING_SIZE = 256;
constexpr uint8_t FRAME_SIZE = 25;

uint8_t ringBuf[RING_SIZE];
volatile uint16_t ringHead = 0;
volatile uint16_t ringTail = 0;
uint8_t rxByte = 0;

uint8_t frame[FRAME_SIZE];
uint8_t lastFrame[FRAME_SIZE];
uint8_t framePos = 0;

uint32_t okFrames = 0;
uint32_t badFrames = 0;
uint32_t lastReport = 0;

inline void ringPush(uint8_t _b)
{
    const uint16_t next = static_cast<uint16_t>((ringHead + 1U) % RING_SIZE);
    if (next != ringTail) {
        ringBuf[ringHead] = _b;
        ringHead = next;
    }
}

inline bool ringPop(uint8_t &_b)
{
    if (ringTail == ringHead) {
        return false;
    }
    _b = ringBuf[ringTail];
    ringTail = static_cast<uint16_t>((ringTail + 1U) % RING_SIZE);
    return true;
}

inline void armReceive() { HAL_UART_Receive_IT(&huart3, &rxByte, 1); }

/**
 * @brief 取第 _ch 个通道（0~15）的 11 位数值
 *
 * SBUS 把 16 个 11 位值首尾相接排成比特流（低位在前），
 * 起点在 buf[1]，所以第 _ch 个通道从第 _ch*11 位开始。
 */
inline uint16_t channelOf(const uint8_t *_buf, uint8_t _ch)
{
    const uint16_t bit = static_cast<uint16_t>(_ch * 11U);
    const uint8_t idx = static_cast<uint8_t>(1U + bit / 8U);
    const uint8_t shift = static_cast<uint8_t>(bit % 8U);

    const uint32_t v = static_cast<uint32_t>(_buf[idx])
                     | (static_cast<uint32_t>(_buf[idx + 1U]) << 8)
                     | (static_cast<uint32_t>(_buf[idx + 2U]) << 16);

    return static_cast<uint16_t>((v >> shift) & 0x07FFU);
}

} // namespace

extern "C" void HAL_UART_RxCpltCallback(UART_HandleTypeDef *_huart)
{
    if (_huart->Instance == USART3) {
        ringPush(rxByte);
        armReceive();
    }
}

SbusProbe &SbusProbe::instance()
{
    static SbusProbe probe;
    return probe;
}

void SbusProbe::init() { armReceive(); }

void SbusProbe::poll()
{
    /* 出现过 FE/PE 等错误后 HAL 会把接收停掉，这里自愈 */
    if (huart3.RxState != HAL_UART_STATE_BUSY_RX) {
        armReceive();
    }

    uint8_t b = 0;
    while (ringPop(b)) {
        if (framePos == 0U && b != 0x0FU) {
            continue; /* 丢掉帧头之前的字节 */
        }

        frame[framePos++] = b;
        if (framePos < FRAME_SIZE) {
            continue;
        }

        framePos = 0;
        for (uint8_t i = 0; i < FRAME_SIZE; ++i) {
            lastFrame[i] = frame[i];
        }
        if (frame[FRAME_SIZE - 1U] == 0x00U) {
            okFrames++;
        } else {
            badFrames++;
        }
    }

    const uint32_t now = HAL_GetTick();
    if ((now - lastReport) < 500U) {
        return;
    }
    lastReport = now;

    LOG::info("SBUS",
              "ok=%u bad=%u err=%u b0=%u b24=%u ch0=%u ch1=%u ch2=%u ch3=%u",
              static_cast<unsigned>(okFrames),
              static_cast<unsigned>(badFrames),
              static_cast<unsigned>(huart3.ErrorCode),
              static_cast<unsigned>(lastFrame[0]),
              static_cast<unsigned>(lastFrame[FRAME_SIZE - 1U]),
              static_cast<unsigned>(channelOf(lastFrame, 0)),
              static_cast<unsigned>(channelOf(lastFrame, 1)),
              static_cast<unsigned>(channelOf(lastFrame, 2)),
              static_cast<unsigned>(channelOf(lastFrame, 3)));
}
