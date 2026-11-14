#include "dmimu.hpp"
#include <cstdint>
#include <cstring>
#include <stdint.h>

namespace INS {

float Ins::phrase(const uint8_t *_bytes, float _range, float _offset)
{
    // TODO:
    const uint16_t raw = static_cast<uint16_t>(_bytes[0]) | (static_cast<uint16_t>(_bytes[1]) << 8);
    return static_cast<float>(raw) * _range / 65535.0f + _offset;
}

bool Ins::init(canHandle *_hcan)
{
    // TODO:
    if (_hcan == nullptr || hcan_ != nullptr) { // 空句柄 / 重复初始化
        return false;
    }
    hcan_ = _hcan;

    const uint32_t now = HAL_GetTick(); // 时间基准，避免上电瞬间误判“掉线/卡死”
    lastRxMs_ = now;
    lastChangeMs_ = now;
    lastRequestMs_ = now;
    lastRestartMs_ = now;

    
    if (Can::instance().registerCallback(hcan_, MASTERID, [this](const uint8_t *_frame) { receiveFromISR(_frame); }, 0)
        != HAL_OK) {
        hcan_ = nullptr;
        return false;
    }
    return true;
}

void Ins::receiveFromISR(const uint8_t *_frame)
{
    // TODO:
    std::memcpy(rxFrame_, _frame, 8);
    rxTimeMs_ = HAL_GetTick();
    rxPending_ = true;
}

void Ins::processFrame(const uint8_t *_frame, uint32_t _timeMs)
{
    // TODO:
    if(_frame[0] != 3) {
        return;
    }
    data_.pitch=phrase(&_frame[2], 180.0f, -90.0f);
	data_.yaw=  phrase(&_frame[4], 360.0f, -180.0f);
	data_.roll= phrase(&_frame[6], 360.0f, -180.0f);
    lastChangeMs_ = _timeMs;
    hasData_ = true;
    lastRxMs_ = _timeMs; 
}

bool Ins::transmit(uint8_t _rid, bool _write)
{
    // TODO:
    if (hcan_ == nullptr) {
        return false;
    }
    uint8_t ac = _write ? 1 : 0;
    uint8_t txbuf[8]={0xCC,_rid,ac,0xDD,0,0,0,0};
    Can::instance().transmitData(hcan_, IMUID, txbuf, 8);
    return true;
}

void Ins::update()
{
    taskENTER_CRITICAL();
    //这里需要尽量少的放东西，处理重要数据放在这里，
    //其作用可以暂时理解为当程序走到这里的时候，不完成里面的程序就不会退出去走下面的程序
    // TODO:
    bool hasNewData = false;
    uint8_t localFrame[8] = {0};
    uint32_t localTime = 0;
    if (rxPending_) {
        std::memcpy(localFrame, rxFrame_, 8); // 复印数据
        localTime = rxTimeMs_;
        rxPending_ = false;           // 清空标志
        hasNewData = true;
    }
    taskEXIT_CRITICAL();
    // TODO:
    if (hasNewData) {
        processFrame(localFrame, localTime); // 算角度
    }

    uint32_t now = HAL_GetTick();

    if (hasData_ && (now - lastChangeMs_) >=EULER_STUCK_MS) {
        if ( now - lastRestartMs_ >= RESTART_INTERVAL_MS) {
            
            transmit(0x00, true);
            lastRestartMs_ = now;
            lastChangeMs_ = now; // 重启后，重置上次变化时刻，避免连续重启
        };
    }

    if (now - lastRequestMs_ >= EULER_POLL_MS) {
        if (transmit(0x03, false)) {  // 发请求
            lastRequestMs_ = now;     // 记下发请求的时间
        }
    }
    

}

void Ins::task()
{
    constexpr TickType_t PERIOD = configTICK_RATE_HZ / RX_FREQ > 0 ? configTICK_RATE_HZ / RX_FREQ : 1;
    TickType_t lastWake = xTaskGetTickCount();
    while (true) {
        //这里可以暂时理解为裸机开发，也就是平时使用的whlie循环里的东西
        update();
        vTaskDelayUntil(&lastWake, PERIOD);
    }
}

Data_s Ins::getData() const
{
    taskENTER_CRITICAL();
    // TODO:
    const Data_s copy = data_;
    taskEXIT_CRITICAL();
    return copy;
}

//这里的Online和ready并不冲突，至于为什么，实践的时候会遇到这个问题的
bool Ins::isOnline() const
{
    taskENTER_CRITICAL();
    // TODO:
    const bool online = HAL_GetTick() - lastRxMs_ <= OFFLINE_TIMEOUT_MS;
    taskEXIT_CRITICAL();
    return online;
}

bool Ins::isReady() const
{
    taskENTER_CRITICAL();
    // TODO:
    const bool has = hasData_;
    taskEXIT_CRITICAL();
    const bool ready = isOnline() &&  has;
    return ready;
}

} // namespace INS
