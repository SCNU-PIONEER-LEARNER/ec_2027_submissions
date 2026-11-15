#include "dmimu.hpp"
#include <cstring>

namespace INS {
//
float Ins::phrase(const uint8_t *_bytes, float _range, float _offset)
{
    // TODO:
    /* 小端拼成 16 位：_bytes[0] 是低字节 */
    const uint16_t raw = static_cast<uint16_t>(_bytes[0]) | static_cast<uint16_t>(_bytes[1] << 8);

    /* 线性映射：0→min，65535→max */
    return static_cast<float>(raw) * _range / 65535.0f + _offset;
}

bool Ins::init(canHandle *_hcan)
{
    // TODO:
    if (_hcan == nullptr) {
        return false;
    }
    hcan_ = _hcan;

    Can::instance().registerCallback(hcan_, MASTERID, [this](const uint8_t *_frame) {
        receiveFromISR(_frame);//从中断里接收
    });

    lastRequestMs_ = HAL_GetTick();//记录当前时间，作为最后一次请求的时间
   // transmit(3, false); /* 3 = 欧拉角寄存器，先要一帧 */

    return true;
}

void Ins::receiveFromISR(const uint8_t *_frame)
{
    // TODO:
    if (_frame == nullptr) {
        return;
    }
    //memcpy函数：（目的地，源，拷贝的字节数）
    std::memcpy(rxFrame_, _frame, 8);
    rxTimeMs_ = HAL_GetTick();
    rxPending_ = true;
}
//加工
void Ins::processFrame(const uint8_t *_frame, uint32_t _timeMs)
{
    // TODO:
    /* data[0] = 1 加速度 / 2 角速度 / 3 欧拉角 / 4 四元数，这里只取欧拉角 */
    if (_frame[0] != 3) {
        return;
    }
//取地址符号
    std::memcpy(rawEuler_, &_frame[2], 6);

    const float pitch = phrase(&rawEuler_[0], 180.0f, -90.0f);
    const float yaw   = phrase(&rawEuler_[2], 360.0f, -180.0f);
    const float roll  = phrase(&rawEuler_[4], 360.0f, -180.0f);
   //phrase函数的作用是将原始数据转换为实际的角度值
    if ((pitch != data_.pitch) || (yaw != data_.yaw) || (roll != data_.roll)) {
        lastChangeMs_ = _timeMs;
    }

    data_.pitch = pitch;
    data_.yaw = yaw;
    data_.roll = roll;

    lastRxMs_ = _timeMs;//记录最后一次接收数据的时间
    hasData_ = true;
}

bool Ins::transmit(uint8_t _rid, bool _write)
{
    // TODO:
    if (hcan_ == nullptr) {
        return false;
    }

    /* 命令帧：CC | 寄存器 | 读(0)/写(1) | DD | 4 字节参数 */
    uint8_t buf[8] = {0xCC, _rid, static_cast<uint8_t>(_write), 0xDD, 0, 0, 0, 0};

    return Can::instance().transmitData(hcan_, IMUID, buf, 8) == HAL_OK;
}

void Ins::update()
{
    taskENTER_CRITICAL();
    //这里需要尽量少的放东西，处理重要数据放在这里，
    //其作用可以暂时理解为当程序走到这里的时候，不完成里面的程序就不会退出去走下面的程序
    // TODO:
    const bool pending = rxPending_;
    uint8_t frame[8];
    if (pending) {
        std::memcpy(frame, rxFrame_, 8);
        rxPending_ = false;
    }
    taskEXIT_CRITICAL();

    // TODO:
    if (pending) {
        processFrame(frame, rxTimeMs_);
    }

    const uint32_t now = HAL_GetTick();

    /* 请求模式：每 5ms 要一次欧拉角 */
    if ((now - lastRequestMs_) >= EULER_POLL_MS) {
        lastRequestMs_ = now;
        transmit(3, false);
    }

    /* 数据 5 秒没变化，重新请求，最多每 5 秒一次 */
    if (!isReady()) {
        lastRestartMs_ = now;
        lastRequestMs_ = now;
        transmit(0, true); /* 0 = 重启寄存器，写 1 = 重启 */
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
    const bool online = hasData_ && ((HAL_GetTick() - lastRxMs_) <= OFFLINE_TIMEOUT_MS);
    taskEXIT_CRITICAL();
    return online;
}

bool Ins::isReady() const
{
    taskENTER_CRITICAL();
    // TODO:
const uint32_t now = HAL_GetTick();

    const bool ready = hasData_ &&
                       ((now - lastRxMs_) < RESTART_INTERVAL_MS) &&
                       ((now - lastChangeMs_) < EULER_STUCK_MS);


    taskEXIT_CRITICAL();
    return ready;
}

} // namespace INS