#include "dmimu.hpp"
#include <cmath>
#include <cstring>

namespace INS {

/* ============================================================================
 * phrase() —— 把 2 字节原始值还原成物理量
 * ----------------------------------------------------------------------------
 * 手册表 B-2 公式： raw / 65535 × range − offset
 *   · 俯仰角：range=180, offset=90    → -90 ~ +90°
 *   · 偏航/横滚：range=360, offset=180 → -180 ~ +180°
 * ========================================================================== */
/* ★ 本题填写（原骨架为 // TODO: + return 0.0f） */
float Ins::phrase(const uint8_t *_bytes, float _range, float _offset)
{
    /* 字节序以官方例程 dm_imu.c 为准： raw = pData[1] << 8 | pData[0]
     * 也就是低字节在前（小端），和 CAN 帧里"先低后高"一致。 */
    const uint16_t raw = static_cast<uint16_t>((static_cast<uint16_t>(_bytes[1]) << 8) | _bytes[0]);
    return static_cast<float>(raw) / 65535.0f * _range - _offset;
}

/* ============================================================================
 * init() —— 记住 CAN 句柄并登记 0x11（主机 ID）的接收回调
 * ----------------------------------------------------------------------------
 * IMU 出厂默认：从机 ID = 0x01（设备接收指令用），主机 ID = 0x11（设备发数据用）。
 * 所以注册 MASTERID，收到的就是它主动上报的帧。
 * ========================================================================== */
/* ★ 本题填写（原骨架为 // TODO: + return false） */
bool Ins::init(canHandle *_hcan)
{
    if (_hcan == nullptr) {
        return false;
    }
    hcan_ = _hcan;

    /* 回调跑在中断里：只做"拷 8 字节 + 记时间 + 置标志"，解析交给任务。 */
    Can::instance().registerCallback(hcan_, MASTERID, [this](const uint8_t *_d) {
        std::memcpy(rxFrame_, _d, sizeof(rxFrame_));
        rxTimeMs_ = HAL_GetTick();
        rxPending_ = true;
    });

    return true;
}

/* 中断上下文：把这一帧搬进成员缓冲，等任务来取 */
/* ★ 本题填写（原骨架为 // TODO:，空函数） */
void Ins::receiveFromISR(const uint8_t *_frame)
{
    std::memcpy(rxFrame_, _frame, sizeof(rxFrame_));
    rxTimeMs_ = HAL_GetTick();
    rxPending_ = true;
}

/* ============================================================================
 * processFrame() —— 按 DATA[0] 分类解析（手册表 B-1 / B-2）
 * ----------------------------------------------------------------------------
 *   DATA[0]=1 加速度+温度   =2 角速度   =3 欧拉角   =4 四元数
 *   欧拉角帧布局：[1]保留  [2..3]俯仰  [4..5]偏航  [6..7]横滚
 * ========================================================================== */
/* ★ 本题填写（原骨架为 // TODO:，空函数） */
void Ins::processFrame(const uint8_t *_frame, uint32_t _timeMs)
{
    lastRxMs_ = _timeMs;

    if (_frame[0] != 3) {
        return;   // 本项目只用欧拉角，其它类型直接丢
    }

    std::memcpy(rawEuler_, _frame + 1, sizeof(rawEuler_));

    const Data_s old = data_;
    data_.pitch = phrase(_frame + 2, 180.0f, 90.0f);
    data_.yaw   = phrase(_frame + 4, 360.0f, 180.0f);
    data_.roll  = phrase(_frame + 6, 360.0f, 180.0f);

    hasData_ = true;

    /* 记录"数据最后一次变化"的时刻，用于判断欧拉角是否卡死。
     * 阈值 0.1° 是考虑量化噪声：静止时读数也会有 ±1 LSB 的跳动。 */
    constexpr float CHANGE_EPS = 0.1f;
    if (std::fabs(data_.pitch - old.pitch) > CHANGE_EPS ||
        std::fabs(data_.yaw - old.yaw) > CHANGE_EPS ||
        std::fabs(data_.roll - old.roll) > CHANGE_EPS) {
        lastChangeMs_ = _timeMs;
    }
}

/* ============================================================================
 * transmit() —— 主机 → 设备方向发一帧（请求模式 / 停摆补偿用）
 * ----------------------------------------------------------------------------
 * 格式依据官方例程 示例/MC02_CAN收发例程/DM-IMU/dm_imu.c 的 imu_send_cmd()。
 * reg_id 取值见 dm_imu.h 的 reg_id_e：0=重启 1=加速度 2=角速度 3=欧拉角
 * 4=四元数 11=切主动上报 13=设CAN_ID 14=设主机ID 15=输出选择 254=保存参数
 * ========================================================================== */
/* ★ 本题填写（原骨架为 // TODO: + return false） */
bool Ins::transmit(uint8_t _rid, bool _write)
{
    if (hcan_ == nullptr) {
        return false;
    }

    /* 命令帧格式（官方 dm_imu.c: imu_send_cmd）：
     *   [0]=0xCC 帧头  [1]=reg_id  [2]=ac(0读/1写)  [3]=0xDD 帧尾  [4..7]=data(小端)
     * 注意发给的是**从机 ID**（IMUID=0x01），不是上报用的 MASTERID。 */
    uint8_t tx[8] = { 0xCC, _rid, static_cast<uint8_t>(_write ? 1 : 0), 0xDD, 0, 0, 0, 0 };

    return Can::instance().transmitData(hcan_, IMUID, tx, sizeof(tx)) == HAL_OK;
}

/* ============================================================================
 * update() —— 任务上下文，每 1ms 调一次
 * ----------------------------------------------------------------------------
 * 临界区里只做"把中断攒下的那一帧取走"，解析放到临界区外面，
 * 尽量缩短关中断时间（关太久会影响 CAN 接收和其他中断的响应）。
 * ========================================================================== */
/* ★ 本题填写（原骨架：临界区内一处 TODO + 函数末尾一处 TODO） */
void Ins::update()
{
    uint8_t frame[8] = {};
    bool pending = false;

    taskENTER_CRITICAL();
    if (rxPending_) {
        std::memcpy(frame, rxFrame_, sizeof(frame));
        rxPending_ = false;
        pending = true;
    }
    taskEXIT_CRITICAL();

    if (pending) {
        processFrame(frame, rxTimeMs_);
    }

    /* 停摆补偿：收到过数据但欧拉角长时间不变 → 认为 IMU 侧停发，
     * 每隔 RESTART_INTERVAL_MS 发一帧请求试试（主动上报模式下无害）。 */
    const uint32_t now = HAL_GetTick();
    if (hasData_ && (now - lastChangeMs_) > EULER_STUCK_MS &&
        (now - lastRestartMs_) > RESTART_INTERVAL_MS) {
        lastRestartMs_ = now;
        transmit(0, false);
    }
}

/* ============================================================================
 * task() —— Ins 自己的 FreeRTOS 任务循环，按 RX_FREQ（1kHz）节拍跑
 * ========================================================================== */
void Ins::task()
{
    constexpr TickType_t PERIOD = configTICK_RATE_HZ / RX_FREQ > 0 ? configTICK_RATE_HZ / RX_FREQ : 1;
    TickType_t lastWake = xTaskGetTickCount();
    while (true) {
        update();
        vTaskDelayUntil(&lastWake, PERIOD);
    }
}

/* 取一份姿态快照（临界区里拷贝，避免读到一半被中断改掉） */
/* ★ 本题填写（原骨架：const Data_s copy = {}; 直接返回全 0） */
Data_s Ins::getData() const
{
    taskENTER_CRITICAL();
    const Data_s copy = data_;
    taskEXIT_CRITICAL();
    return copy;
}

/* 在线：收到过数据，且最近 OFFLINE_TIMEOUT_MS 内还有新帧 */
/* ★ 本题填写（原骨架：const bool online = false;） */
bool Ins::isOnline() const
{
    taskENTER_CRITICAL();
    const bool online = hasData_ && (HAL_GetTick() - lastRxMs_) <= OFFLINE_TIMEOUT_MS;
    taskEXIT_CRITICAL();
    return online;
}

/* 就绪：在线，且欧拉角最近有变化（没卡死）。
 * 与 isOnline 的区别：在线只说明"有帧进来"，就绪还要求"数据是活的"。 */
/* ★ 本题填写（原骨架：const bool ready = false;） */
bool Ins::isReady() const
{
    taskENTER_CRITICAL();
    const uint32_t now = HAL_GetTick();
    const bool ready = hasData_ && (now - lastRxMs_) <= OFFLINE_TIMEOUT_MS &&
                       (now - lastChangeMs_) <= EULER_STUCK_MS;
    taskEXIT_CRITICAL();
    return ready;
}

} // namespace INS
