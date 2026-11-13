#include "DJIMotor.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>

using namespace MOTOR;

/* ============================================================================
 * init() —— 绑定 CAN 句柄与电机 ID，并登记反馈回调
 * ----------------------------------------------------------------------------
 *   ① 记住用哪个 CAN、哪台电机（regInfo_）
 *   ② 算出这台电机在"一帧控 4 台"里占第几个 2 字节槽位（txSlot，0~3）
 * 本版本只做发送，不注册反馈回调。
 * ========================================================================== */
void DJIMotor::init(canHandle *_hcan, uint8_t _id)
{
    regInfo_.hcan = _hcan;
    regInfo_.id = _id;

    /* 槽位 = (ID-1) % 4。一帧 8 字节装 4 台，每台 2 字节：
     *   ID 1 → 0 号槽（tx[0..1]）  ID 2 → 1 号槽（tx[2..3]）
     *   ID 3 → 2 号槽              ID 4 → 3 号槽
     *   ID 5 → 回到 0 号槽，但换用 0x2FE 那帧 */
    /* ★ 本题填写开始（原骨架为 // TODO:） */
    regInfo_.txSlot = static_cast<uint8_t>((_id - 1) % 4);
    /* ★ 本题填写结束 */
}

/* ============================================================================
 * cmdCurrent() —— 把目标电流填进对应的 2 字节槽位并发送
 * ----------------------------------------------------------------------------
 * 返回 true 表示帧已进发送邮箱；false 可能是"1ms 内重复调用"或发送失败。
 * ========================================================================== */
bool DJIMotor::cmdCurrent(float _current)
{
    if (HAL_GetTick() - lastTxMs_ < 1)
        return false;

    lastTxMs_ = HAL_GetTick();
    uint8_t tx[8] = { 0 };
    uint16_t code = currentToCode(_current);

    /* ★ 本题填写开始（原骨架为 // TODO: + return false） */
    /* 大端：高字节在前。填错槽位就会去控另一台电机。 */
    tx[regInfo_.txSlot * 2]     = static_cast<uint8_t>(code >> 8);
    tx[(regInfo_.txSlot * 2) + 1] = static_cast<uint8_t>(code & 0xFF);

    return Can::instance().transmitData(regInfo_.hcan, txStdID(), tx, 8) == HAL_OK;
    /* ★ 本题填写结束 */
}

/* 按 ID 选控制帧：ID 1~4 用 0x1FE，5~7 用 0x2FE
 * ★ 本题填写：原骨架只有声明 uint32_t txStdID(); 没有定义（不写会链接报错） */
uint32_t DJIMotor::txStdID()
{
    return regInfo_.id <= 4 ? TX_STDID_1_4 : TX_STDID_5_7;
}

uint16_t DJIMotor::currentToCode(float _current)
{
    constexpr float NUM_PER_AMP = CURRENT_CODE_MAX / CURRENT_MAX;
    float code = std::clamp(_current, -CURRENT_MAX, CURRENT_MAX) * NUM_PER_AMP;
    int32_t v = static_cast<int32_t>(std::lround(code));
    v = v > 16384 ? 16384 : (v < -16384 ? -16384 : v);
    return static_cast<uint16_t>(static_cast<int16_t>(v));
}
