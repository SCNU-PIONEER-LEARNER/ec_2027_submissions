#include "dmimu.hpp"
#include <cmath>
#include <cstring>

namespace INS 
{
    namespace 
{

constexpr uint8_t REG_REBOOT = 0;     
constexpr uint8_t REG_EULER = 3;    
constexpr bool CMD_READ = false;      
constexpr bool CMD_WRITE = true;    

// 欧拉角换算：value = raw * span / 65535 + offset
constexpr float PITCH_SPAN = 180.0f, PITCH_OFF = -90.0f;  
constexpr float YAW_SPAN = 360.0f, YAW_OFF = -180.0f;     
constexpr float ROLL_SPAN = 360.0f, ROLL_OFF = -180.0f;   

// 原始数据冻结超过 EULER_STUCK_MS 判定为 IMU 卡死，重启它。
// 如果现场发现静止时被误判重启，把这个改成 false。
constexpr bool ENABLE_STUCK_RESTART = true;
} // namespace

float Ins::phrase(const uint8_t *_bytes, float _range, float _offset)
{
    // TODO:
    const uint16_t raw = static_cast<uint16_t>(_bytes[0]) | (static_cast<uint16_t>(_bytes[1]) << 8);
    return (static_cast<float>(raw) * (_range / 65535.0f)) + _offset;
}

bool Ins::init(canHandle *_hcan)
{
    // TODO:
    if (_hcan == nullptr) 
    {
        return false;
    }
    hcan_ = _hcan;

    // IMU 的应答帧发到 MASTERID(0x11)，长度固定 8 字节
    // 第一帧不用在这里要：update() 5ms 内一定会发第一次读请求
    return Can::instance().registerCallback(
                   hcan_, MASTERID, [this](const uint8_t *_data) { receiveFromISR(_data); }, 8) == HAL_OK;
}

void Ins::receiveFromISR(const uint8_t *_frame)
{
    // TODO:
    // 中断上下文：只搬运 + 置标志，解析留给任务
    std::memcpy(rxFrame_, _frame, sizeof(rxFrame_));
    rxTimeMs_ = HAL_GetTick();
    __DMB();          
    rxPending_ = true; 
}

void Ins::processFrame(const uint8_t *_frame, uint32_t _timeMs)
{
    // TODO:
    if (_frame[0] != REG_EULER) 
    {
        return; 
    }

    
    Data_s d{};
    d.pitch = phrase(&_frame[2], PITCH_SPAN, PITCH_OFF);
    d.yaw = phrase(&_frame[4], YAW_SPAN, YAW_OFF);
    d.roll = phrase(&_frame[6], ROLL_SPAN, ROLL_OFF);

    const bool changed = std::memcmp(_frame + 2, rawEuler_, sizeof(rawEuler_)) != 0;
    
    if (changed) 
    {
        std::memcpy(rawEuler_, _frame + 2, sizeof(rawEuler_));
    }

    taskENTER_CRITICAL();
    data_ = d;
    hasData_ = true;
    lastRxMs_ = _timeMs;
    
    if (changed) 
    {
        lastChangeMs_ = _timeMs;
    }
    taskEXIT_CRITICAL();
}

bool Ins::transmit(uint8_t _rid, bool _write)
{
    // TODO:
    if (hcan_ == nullptr) 
    {
        return false; 
    }
    uint8_t tx[8] = { 0xCC, _rid, static_cast<uint8_t>(_write ? 0x01U : 0x00U), 0xDD, 0x00, 0x00, 0x00,
                      0x00 };
    return Can::instance().transmitData(hcan_, IMUID, tx, sizeof(tx)) == HAL_OK;
}

void Ins::update()
{
    uint8_t frame[8] = {};
    bool pending = false;
    uint32_t rxTime = 0;

    taskENTER_CRITICAL();
    //这里需要尽量少的放东西，处理重要数据放在这里，
    //其作用可以暂时理解为当程序走到这里的时候，不完成里面的程序就不会退出去走下面的程序
    // TODO:
    pending = rxPending_;
    if (pending) 
    {
        std::memcpy(frame, rxFrame_, sizeof(frame));
        rxTime = rxTimeMs_;
        rxPending_ = false; 
    }
    taskEXIT_CRITICAL();
    // TODO:
    if (pending) 
    {
        processFrame(frame, rxTime);
    }

    const uint32_t now = HAL_GetTick();

    // 每 5ms 要一次欧拉角 → 200Hz，够闭环又不占满总线
    if (now - lastRequestMs_ >= EULER_POLL_MS) 
    {
        lastRequestMs_ = now;
        transmit(REG_EULER, CMD_READ);
    }

    // 掉线：每 5s 试着重启一次
    if (!isOnline() && (now - lastRestartMs_) >= RESTART_INTERVAL_MS) 
    {
        lastRestartMs_ = now;
        transmit(REG_REBOOT, CMD_WRITE);
    }

    // 在线但原始值冻结超过 5s：同样判定为卡死
    if (ENABLE_STUCK_RESTART && hasData_ && (now - lastChangeMs_) >= EULER_STUCK_MS &&
        (now - lastRestartMs_) >= RESTART_INTERVAL_MS) 
    {
        lastRestartMs_ = now;
        transmit(REG_REBOOT, CMD_WRITE);
    }
}

void Ins::task()
{
    constexpr TickType_t PERIOD = configTICK_RATE_HZ / RX_FREQ > 0 ? configTICK_RATE_HZ / RX_FREQ : 1;
    TickType_t lastWake = xTaskGetTickCount();
    while (true)
    {
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
    // hasData_ 必须有：否则上电头 100ms 会因为 lastRxMs_=0 误判"在线"
    const bool online = hasData_ && (HAL_GetTick() - lastRxMs_ < OFFLINE_TIMEOUT_MS);
    taskEXIT_CRITICAL();
    return online;
}

bool Ins::isReady() const
{
    // TODO:
    if (!isOnline()) 
    {
        return false;
    }

    const Data_s d = getData();
    return (std::fabs(d.pitch) <= 90.0f) && (std::fabs(d.yaw) <= 180.0f) &&
           (std::fabs(d.roll) <= 180.0f);
}

} // namespace INS
