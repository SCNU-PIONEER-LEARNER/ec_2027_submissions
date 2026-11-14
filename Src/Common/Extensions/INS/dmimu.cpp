#include "dmimu.hpp"
#include <cstring>

namespace INS {

float Ins::phrase(const uint8_t *_bytes, float _range, float _offset)
{
    // TODO:
    const uint16_t raw = static_cast<uint16_t>(_bytes[0]) |
                         static_cast<uint16_t>(static_cast<uint16_t>(_bytes[1]) << 8);
    return static_cast<float>(raw) / 65535.0f * _range + _offset;

    return 0.0f;
}

bool Ins::init(canHandle *_hcan)
{
    // TODO:
    if (_hcan == nullptr) {
        return false;
    }
    if (hcan_ != nullptr) {
        return false;
    }

    const uint32_t now = HAL_GetTick();
    lastRxMs_      = now;
    lastChangeMs_  = now;
    lastRestartMs_ = now;
    lastRequestMs_ = now;
    hasData_       = false;
    rxPending_     = false;
    data_          = Data_s{};
    std::memset(rawEuler_, 0, sizeof(rawEuler_));

    const HAL_StatusTypeDef result = Can::instance().registerCallback(
        _hcan, MASTERID,
        [](const uint8_t *_frame){Ins::instance().receiveFromISR(_frame);}, 8);
    if (result != HAL_OK) {
        return false;
    }

    hcan_ = _hcan;
    return true;

    return false;
}

void Ins::receiveFromISR(const uint8_t *_frame)
{
    // TODO:
    // _frame 指向中断栈上的临时缓冲，出了回调就失效，必须立刻拷走
    std::memcpy(rxFrame_, _frame, sizeof(rxFrame_));
    rxTimeMs_ = HAL_GetTick();
    // 最后才挂牌：数据和时间戳都写完了
    rxPending_ = true;
}

void Ins::processFrame(const uint8_t *_frame, uint32_t _timeMs)
{
    // TODO:
    constexpr uint8_t EULER_ID = 3;   // 只处理欧拉角数据帧
    if (_frame[0] != EULER_ID) {
        return;
    }

    // 先跟上一帧比（此时 rawEuler_ 还是旧值），再覆盖
    const bool changed = (std::memcmp(rawEuler_, _frame + 2, sizeof(rawEuler_)) != 0);
    const bool reconnected = ((_timeMs - lastRxMs_) > OFFLINE_TIMEOUT_MS);

    std::memcpy(rawEuler_, _frame + 2, sizeof(rawEuler_));

    data_.pitch = phrase(rawEuler_ + 0, 180.0f, -90.0f);
    data_.yaw   = phrase(rawEuler_ + 2, 360.0f, -180.0f);
    data_.roll  = phrase(rawEuler_ + 4, 360.0f, -180.0f);

    lastRxMs_ = _timeMs;
    if (changed || reconnected) {
        lastChangeMs_ = _timeMs;
        hasData_      = true;
    }
}

bool Ins::transmit(uint8_t _rid, bool _write)
{
    // TODO:
    if (hcan_ == nullptr) {
        return false;
    }
    uint8_t tx[8] = {0xCC, _rid, static_cast<uint8_t>(_write ? 1u : 0u), 0xDD, 0, 0, 0, 0};
    return Can::instance().transmitData(hcan_, IMUID, tx, sizeof(tx)) == HAL_OK;

    return false;
}

void Ins::update()
{
    taskENTER_CRITICAL();
    //这里需要尽量少的放东西，处理重要数据放在这里，
    //其作用可以暂时理解为当程序走到这里的时候，不完成里面的程序就不会退出去走下面的程序
    // TODO:
    uint8_t frame[8] = {0};
    uint32_t frameTime = 0;
    bool pending = false;
    if (rxPending_) {
        for (uint8_t i = 0; i < 8; ++i) {
            frame[i] = rxFrame_[i];
        }
        frameTime  = rxTimeMs_;
        rxPending_ = false;
        pending    = true;
    }
    taskEXIT_CRITICAL();

    // TODO:
    if (pending) {
        processFrame(frame, frameTime);
    }

    const uint32_t now    = HAL_GetTick();
    if(now-lastRestartMs_>=RESTART_INTERVAL_MS)
    {
        
        if(!isReady()||!isOnline())
        {
            transmit(0xFF,true);
            lastRestartMs_ = now;
        }
    } else if ((now - lastRequestMs_) >= EULER_POLL_MS) {
        // 没人主动推数据时，每 5 ms 主动要一次欧拉角
        lastRequestMs_ = now;
        transmit(3, false);
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
    const uint32_t now = HAL_GetTick();
    const bool online = ((now - lastRxMs_) <= OFFLINE_TIMEOUT_MS);
    taskEXIT_CRITICAL();
    return online;
}

bool Ins::isReady() const
{
    taskENTER_CRITICAL();
    // TODO:
    const uint32_t now = HAL_GetTick();
    const bool ready = ((now - lastRxMs_) <= OFFLINE_TIMEOUT_MS) && hasData_ &&
                       ((now - lastChangeMs_) < EULER_STUCK_MS);
    taskEXIT_CRITICAL();
    return ready;
}

} // namespace INS
