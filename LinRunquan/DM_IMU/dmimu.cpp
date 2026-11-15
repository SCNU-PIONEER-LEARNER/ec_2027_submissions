#include "dmimu.hpp"
#include <cstring>

namespace INS {

float Ins::phrase(const uint8_t *_bytes, float _range, float _offset)
{
    // TODO:
    // 两字节小端拼成原始值（0~65535），映射到 [-range/2, +range/2]，再叠加校准偏置
    const uint16_t raw = static_cast<uint16_t>(_bytes[0]) | (static_cast<uint16_t>(_bytes[1]) << 8);
    return static_cast<float>(raw) * _range / 65535.0f - _range * 0.5f + _offset;
    return 0.0f;
}

bool Ins::init(canHandle *_hcan)
{
    // TODO:
    if (_hcan == nullptr || hcan_ != nullptr) {
        return false;
    }

    const HAL_StatusTypeDef result = Can::instance().registerCallback(
            _hcan, MASTERID, [this](const uint8_t *_frame) { receiveFromISR(_frame); }, 8);
    if (result != HAL_OK) {
    return false;
    }

    hcan_ = _hcan;
    lastRxMs_ = HAL_GetTick();
    lastChangeMs_ = lastRxMs_;
    lastRequestMs_ = lastRxMs_;
    lastRestartMs_ = lastRxMs_;
    rxPending_ = false;
    hasData_ = false;
    return true;
}

void Ins::receiveFromISR(const uint8_t *_frame)
{
    // TODO:
    // 中断里只做搬运：拷贝 + 打时间戳 + 置标志，解码留给任务
    if (_frame == nullptr) {
        return;
    }
    std::memcpy(rxFrame_, _frame, sizeof(rxFrame_));
    rxTimeMs_ = HAL_GetTick();
    rxPending_ = true;
}

void Ins::processFrame(const uint8_t *_frame, uint32_t _timeMs)
{
    // TODO:
     if (_frame[0] != 3) {
        return; // 不是欧拉角帧，整帧丢弃
    }

    // 用帧自带的时间戳判断这一帧之前是否已经离线
    const bool wasOffline = !hasData_ || (_timeMs - lastRxMs_ > OFFLINE_TIMEOUT_MS);

    // 只比 6 个欧拉字节：byte1 是温度，温度变了不算姿态变化
    const bool changed = std::memcmp(rawEuler_, &_frame[2], sizeof(rawEuler_)) != 0;
    std::memcpy(rawEuler_, &_frame[2], sizeof(rawEuler_));

    data_.pitch = phrase(&rawEuler_[0], 180.0f, 0.0f); // 跨度 180° → [-90, 90]
    data_.yaw = phrase(&rawEuler_[2], 360.0f, 0.0f);   // 跨度 360° → [-180, 180]
    data_.roll = phrase(&rawEuler_[4], 360.0f, 0.0f);  // 同 yaw

    lastRxMs_ = _timeMs;
    hasData_ = true;
    if (changed || wasOffline) {
        lastChangeMs_ = _timeMs; // 数据有变化（或刚从离线恢复），重新开始观察窗
    }
    
}

bool Ins::transmit(uint8_t _rid, bool _write)
{
    // TODO:
    if (hcan_ == nullptr) {
        return false;
    }

    uint8_t tx[8] = { 0xCC, _rid, static_cast<uint8_t>(_write ? 1 : 0), 0xDD, 0, 0, 0, 0 };
    return Can::instance().transmitData(hcan_, IMUID, tx, sizeof(tx)) == HAL_OK;
   
   
}

void Ins::update()
{
    taskENTER_CRITICAL();
    //这里需要尽量少的放东西，处理重要数据放在这里，
    //其作用可以暂时理解为当程序走到这里的时候，不完成里面的程序就不会退出去走下面的程序
    // TODO: 
    if (rxPending_) {
        rxPending_ = false;
        processFrame(rxFrame_, rxTimeMs_);
    }
    taskEXIT_CRITICAL();

    // TODO:
    const uint32_t now = HAL_GetTick();

    const bool online = hasData_ && (now - lastRxMs_ < OFFLINE_TIMEOUT_MS);
    const bool frozen = online && (now - lastChangeMs_ >= EULER_STUCK_MS);

    // 冻结：先记时间再发，失败也占住这 5 秒窗口，避免 1kHz 连续重试
    if (frozen && (now - lastRestartMs_ >= RESTART_INTERVAL_MS)) {
        lastRestartMs_ = now;
        if (transmit(0x00, true)) {
            hasData_ = false; // 重启后旧姿态作废，等新数据
        }
    }

    // 兜底轮询：5ms 问一次欧拉角
    if (now - lastRequestMs_ >= EULER_POLL_MS) {
        lastRequestMs_ = now;
        transmit(0x03, false);
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
    const Data_s copy = {};
    taskEXIT_CRITICAL();
    return copy;
}

//这里的Online和ready并不冲突，至于为什么，实践的时候会遇到这个问题的
bool Ins::isOnline() const
{
    taskENTER_CRITICAL();
    // TODO:
     const bool online = hasData_ && (HAL_GetTick() - lastRxMs_ < OFFLINE_TIMEOUT_MS);
    const bool online = false;
    taskEXIT_CRITICAL();
    return online;
}

bool Ins::isReady() const
{
    taskENTER_CRITICAL();
    // TODO:
    const uint32_t now = HAL_GetTick();
    const bool ready = hasData_ && (now - lastRxMs_ < OFFLINE_TIMEOUT_MS)
                       && (now - lastChangeMs_ < EULER_STUCK_MS);
    const bool ready = false;
    taskEXIT_CRITICAL();
    return ready;
}

} // namespace INS
