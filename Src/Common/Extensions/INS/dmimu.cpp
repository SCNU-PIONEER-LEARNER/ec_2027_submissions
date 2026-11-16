#include "dmimu.hpp"
#include <cstring>

namespace INS {
//翻译函数
float Ins::phrase(const uint8_t *_bytes, float _range, float _offset)
{//将两个字节的IMU数据转换为浮点数，范围为_range，偏移量为_offset（协议里的imu数据是16位的，范围是0~65535，所以要除以65535.0f）
    const int raw = (_bytes[1] << 8) | _bytes[0];//高字节与低字节组合成一个16位的整数
    return static_cast<float>(raw) * _range / 65535.0f - _offset;
}//把拼接出来的row值映射到角度上，先乘以浮点数range保证进入小数运算，再除65535进行角度映射，最后减去offset得到偏移值

bool Ins::init(canHandle *_hcan)
{       //防止重复初始化，_hcan为空指针，或者已经初始化过了
    if (_hcan == nullptr || hcan_ != nullptr) {
        return false;
    }

    hcan_ = _hcan;//标记用哪条can通道
            //注册回调函数，接收IMU数据
    if (Can::instance().registerCallback(
            _hcan,//要用哪条can通道
            MASTERID,//反馈id
            [this](const uint8_t *_frame) { receiveFromISR(_frame); },//收到之后调用这个函数
            8) != HAL_OK) {
        hcan_ = nullptr;
        return false;
    } 

    rxPending_ = false;
    hasData_ = false;
    lastRxMs_ = 0;

    return true;
}

void Ins::receiveFromISR(const uint8_t *_frame)
{
    if (_frame == nullptr) {
        return;
    }
        //把8个字节的IMU数据拷贝到rxFrame_数组中，并记录接收时间和标记接收状态(目的地, 源地址, 拷贝多少字节);
    std::memcpy(rxFrame_, _frame, sizeof(rxFrame_));
    rxTimeMs_ = HAL_GetTick();//记录
    rxPending_ = true;
}

//拆包函数
void Ins::processFrame(const uint8_t *_frame, uint32_t _timeMs)
{   //如果frame【0】是角度帧则继续解析（在手册中有说到欧拉角是Data{0}=3)
    if (_frame == nullptr || _frame[0] != 3) {
        return;
    }

    uint8_t nextEuler[6];
    std::memcpy(nextEuler, _frame + 2, sizeof(nextEuler));//从frame【2】开始抄六个字节的数据给nextEuler

    const bool changed =//第一次收到数据，或者这次6个字节角度和上次不一样，就让change变成true
        !hasData_ || (std::memcmp(rawEuler_, nextEuler, sizeof(rawEuler_)) != 0);

    const float pitch = phrase(_frame + 2, 180.0f, 90.0f);
    const float yaw   = phrase(_frame + 4, 360.0f, 180.0f);
    const float roll  = phrase(_frame + 6, 360.0f, 180.0f);

    taskENTER_CRITICAL();//FreeRTOS的关门宏定义，进入临界区，禁止任务切换，挡住会调用 FreeRTOS 的中断；
    std::memcpy(rawEuler_, nextEuler, sizeof(rawEuler_));//将得到的6个字节存进rawEuler，供下一帧比较
    data_.pitch = pitch;//更新，将算好的角度放到对应要求上
    data_.yaw   = yaw;
    data_.roll  = roll;
    hasData_ = true;//标记已经得到过有效数据
    lastRxMs_ = _timeMs;//这一帧收到时的时间，进行时间更新
    if (changed) {
        lastChangeMs_ = _timeMs;//如果确认角度改变，就把时间更新
    }
    taskEXIT_CRITICAL();//退出临界区，恢复；
}

bool Ins::transmit(uint8_t _rid, bool _write)
{
    // 还没 init 成功，没有 CAN 句柄，直接失败
    if (hcan_ == nullptr) {
        return false;
    }

    uint8_t frame[8] = {
        0xCC,
        _rid,
        static_cast<uint8_t>(_write),
        0xDD,
        0, 0, 0, 0,
    };

    // 命令发给 IMU 自己的 CAN ID：IMUID = 0x01
    return Can::instance().transmitData(hcan_, IMUID, frame, 8) == HAL_OK;
}

void Ins::update()
{
    uint8_t frame[8]; // 自己的桌子：放这一帧的 8 字节
    uint32_t timeMs = 0;// 这一帧的收到时间
    bool pending = false; // 有没有新帧

    // 临界区只做拷贝：把 CAN 中断收到的最新一帧拿出来
    taskENTER_CRITICAL();
    pending = rxPending_;
    if (pending) {
        std::memcpy(frame, rxFrame_, sizeof(frame));//如果有新帧把 rxFrame_ 的 8 个字节拷贝到局部 frame。
        timeMs = rxTimeMs_;//拷贝一下时间
        rxPending_ = false;//这一帧我已经拿走了，不要再重复处理
    }
    taskEXIT_CRITICAL();

    // 解析放在临界区外，避免长时间关中断
    if (pending) {
        processFrame(frame, timeMs);
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
    // App 来取角度，临界区里复制，避免读到一半
    taskENTER_CRITICAL();
    const Data_s copy = data_;
    taskEXIT_CRITICAL();
    return copy;
}

//这里的Online和ready并不冲突，至于为什么，实践的时候会遇到这个问题的
bool Ins::isOnline() const
{
    // 100ms 内收到过有效帧，就算在线
    taskENTER_CRITICAL();
    const bool online =
        hasData_ && (HAL_GetTick() - lastRxMs_ < OFFLINE_TIMEOUT_MS);
    taskEXIT_CRITICAL();
    return online;
}

bool Ins::isReady() const
{
    // 在线 + 5s 内角度变化过，才算“可以用”
    taskENTER_CRITICAL();
    const uint32_t now = HAL_GetTick();
    const bool ready =
        hasData_ &&
        (now - lastRxMs_ < OFFLINE_TIMEOUT_MS) &&
        (now - lastChangeMs_ < EULER_STUCK_MS);
    taskEXIT_CRITICAL();
    return ready;
}

} // namespace INS
