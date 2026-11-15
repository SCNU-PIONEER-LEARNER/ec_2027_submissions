#include "dmimu.hpp"
#include <cstring>
//偏航角/横滚角
namespace INS {
//字节 → 数值
float Ins::phrase(const uint8_t *_bytes, float _range, float _offset)
{
    // TODO:
     // 1) 两字节 → uint16（小端：低字节在前）
    const uint16_t raw = (static_cast<uint16_t>(_bytes[0])) | (static_cast<uint16_t>(_bytes[1]) << 8);
     // 2) 0~65535 线性映射到 [-_offset, _range - _offset]
    return (static_cast<float>(raw) * _range / 65535.0f) - _offset;
}
//把"CAN2 上 ID=0x11 的 8 字节帧"这条线，接到自己的 receiveFromISR() 上，并把内部状态清零。
bool Ins::init(canHandle *_hcan)
{
    // TODO:
    if (_hcan == nullptr || hcan_ != nullptr) return false;  // 空句柄 / 重复初始化一律拒绝
    hcan_ = _hcan; //保存句柄
    //注册接收回调 ← 核心
    if (Can::instance().registerCallback(_hcan,MASTERID,[this](const uint8_t *f){receiveFromISR(f);},8)!=HAL_OK)
    {hcan_ = nullptr;
     // 失败要回滚，否则对象处于"半初始化"状态
    return false;}
    data_={};
    std::memset(rawEuler_, 0, sizeof(rawEuler_));  
    std::memset(rxFrame_, 0, sizeof(rxFrame_));
    rxTimeMs_ = 0;
    lastRxMs_ = HAL_GetTick();
    lastChangeMs_ = HAL_GetTick();
    lastRequestMs_ = HAL_GetTick();
    lastRestartMs_ = HAL_GetTick();
    return true;
}
/*参数	值	含义
_hcan	&HCAN2	用哪路 CAN 收
_stdid	MASTERID = 0x11	模块回传用的标准帧 ID
_pCallback	lambda	收到帧后进哪个函数
_expectedLength	8	只要 8 字节的完整帧，DLC≠8 的丢掉*/
void Ins::receiveFromISR(const uint8_t *_frame)
{
    // TODO:
    if (_frame[0] != 0x03) return;   // 0.5) 只要欧拉角帧，其它帧类型直接丢，D:\ec_2027_submissions\test\ins\test_dmimu.cpp
    std::memcpy(rxFrame_,_frame,8);  // 1) 拷 8 字节（临时指针 → 自己的缓冲区），把一块内存里的 N 个字节，原样搬到另一块内存
                                     //把中断递进来的 8 个字节，抄到你自己的成员数组 
    rxTimeMs_ = HAL_GetTick();       // 2) 打时间戳
    rxPending_ =true ;               // 3) 通知任务
}
/*CAN2 硬件收到 ID=0x11 的帧
        ↓  （中断！CPU 此刻打断了别的任务）
Can::callbackFromISR()  →  找到你注册的 lambda  →  Ins::receiveFromISR(frame)*/
void Ins::processFrame(const uint8_t *_frame, uint32_t _timeMs)
{
    // TODO:
    uint8_t fresh[6];
    std::memcpy(fresh,&_frame[2],sizeof(fresh));
    // 1) 只取欧拉角 6 字节（跳过 [0] 帧类型、[1] 温度）
    //    先拷到临时缓冲，方便"比较后再覆盖"
    //    帧内顺序：[2..3]=pitch(180°/90°偏置) [4..5]=yaw [6..7]=roll(360°/180°偏置)
    data_.pitch = phrase(&_frame[2],180.0f , 90.0f);
    data_.yaw  =phrase(&_frame[4],360.0f,180.0f);
    data_.roll =phrase(&_frame[6],360.0f,180.0f);
    // 2) 和上一帧的原始字节比较：有变化 / 重新上线 → 开启新的观察窗口
    //    只比欧拉角 6 字节，温度 [1] 变化不算"数据活着"
    const bool reconnect = !hasData_ || (_timeMs - lastRxMs_ >= OFFLINE_TIMEOUT_MS);
    if (reconnect || std::memcmp(fresh, rawEuler_, sizeof(fresh)) != 0) {
        std::memcpy(rawEuler_, fresh, sizeof(fresh));
        lastChangeMs_ = _timeMs;
    }
    // 3) 记录"活着"的证据
    hasData_ = true;
    lastRxMs_ = _timeMs;    
}
 //CAN 中断里搬回来的那 8 个原始字节（全是 0~255 的乱码），翻译成能直接用的 pitch / yaw / roll（单位为度）

/*是 INS 模块负责发送的函数。前面的 receiveFromISR 和 processFrame 负责收数据；
transmit 则反过来，由主板往 IMU 发指令。
主板 ──transmit()──> CAN2, ID=0x01 ──> DM-IMU-L1
主板 <──receiveFromISR()── CAN2, ID=0x11 ── DM-IMU-L1*/


bool Ins::transmit(uint8_t _rid, bool _write) 
{
    // TODO:
    if (hcan_ == nullptr) return false;   // 放在函数第一行
    uint8_t a[8] ={0};
    a[0]=0xCC;
    a[3]=0xDD;    
    if(_write){
        a[1]=0; //指向哪一格
        a[2]=1; //要读还是要写,写

    }else
    {
        a[1]=_rid; //请把第 3 号数据块（欧拉角）发给我”
        a[2]=0; //读
    }

    return Can::instance().transmitData(hcan_,IMUID,a,8)==HAL_OK;
    //交给 CAN 发出去，并如实汇报结果
}

//中断抢下来的数据取走 → 解码 → 给模块做健康检查并决定要不要补发指令。


void Ins::update()
{ uint8_t a[8]{};
  uint32_t ts= 0;
  bool got;
    taskENTER_CRITICAL();
    //这里需要尽量少的放东西，处理重要数据放在这里，
    //其作用可以暂时理解为当程序走到这里的时候，不完成里面的程序就不会退出去走下面的程序
    // TODO:
     // 只取走 + 清标记
     got = rxPending_;
     if (got) { std::memcpy(a, rxFrame_, 8); 
        ts = rxTimeMs_; rxPending_ = false; }
    taskEXIT_CRITICAL();

    // TODO:
     if (got) processFrame(a, ts);                      // ② 解码（临界区外）

    const uint32_t now = HAL_GetTick();                // ③ 体检 + 调度 // 现在几点（开机以来的 ms 数）
    const bool online = hasData_ && now - lastRxMs_     < OFFLINE_TIMEOUT_MS; //now - lastRxMs_     < OFFLINE_TIMEOUT_MS一个"距离上次重启过了多久"的秒表读数
    const bool ready  = online   && now - lastChangeMs_ < EULER_STUCK_MS;    //
   //online = hasData_ && now - lastRxMs_     < OFFLINE_TIMEOUT_MS;  // 100ms：还有没有收到帧
    //ready  = online   && now - lastChangeMs_ < EULER_STUCK_MS;      // 5000ms：收到的数值还在不在变
       if (online && !ready && now - lastRestartMs_ >= RESTART_INTERVAL_MS) {
        lastRestartMs_ = now;          // 失败也记时间，避免 5ms 一次狂刷
        if (transmit(0, true)) {       // 卡住了 → 重启
            hasData_ = false;          // 设备马上重启，等它重新上线
            lastRxMs_ = now;
            lastChangeMs_ = now;       // 重新开一个观察窗口
        }
    }
    if ((online || !hasData_) && now - lastRequestMs_ >= EULER_POLL_MS) {
        (void)transmit(3, false); lastRequestMs_ = now;   // 平时 → 每 5ms 要一次欧拉角
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

Data_s Ins::getData() const  //读数接口
{
    taskENTER_CRITICAL();
    // TODO:
    const Data_s copy = data_;
    taskEXIT_CRITICAL();
    return copy;
}

//这里的Online和ready并不冲突，至于为什么，实践的时候会遇到这个问题的
bool Ins::isOnline() const //最近有没有收到新帧
{
    taskENTER_CRITICAL();
    // TODO:
    const bool online =hasData_&& (HAL_GetTick() - lastRxMs_ < OFFLINE_TIMEOUT_MS) ; //减法 < 超时
    //数据没超时
    taskEXIT_CRITICAL();
    return online;
}

//现在 getData() 拿到的角度，能不能放心用来算控制量？收到的角度可信且是活的吗？
bool Ins::isReady() const
{
    taskENTER_CRITICAL();
    // TODO:
    //临界区：只做"抄出来"这个动作，短平快
    const bool a=hasData_;
    const uint32_t b=lastRxMs_;
    const uint32_t c=lastChangeMs_;
    const uint32_t d=HAL_GetTick();
    
    taskEXIT_CRITICAL();
    const bool online =a&&(d-b<OFFLINE_TIMEOUT_MS);
    const bool fresh = d-c<EULER_STUCK_MS;   // 与 update() 的判据保持一致

    return online && fresh;
}

}// namespace INS
