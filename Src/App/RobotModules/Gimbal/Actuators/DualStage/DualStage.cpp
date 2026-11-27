#include "sdkconfig.h"
#include "Gimbal.hpp"
#include "MotorCommonMacros.hpp"
#include "DualStage.hpp"
#include "PidBasic.hpp"
#include "DM4310.hpp"
#include "./State/StopState.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>

using namespace GIMBAL;
using namespace PINYMOTOR;

static FILTER::LPF gimbalFilterOutput[3]{
    FILTER::LPF(1000.f, 4.f),
    FILTER::LPF(1000.f, 10.f),
    FILTER::LPF(1000.f, 4.f),
};

DualStage::DualStage()
{
    for (uint8_t i = 0; i < 3; i++) {
        gimbalFilterOutput_._[i] = &gimbalFilterOutput[i];
    }

    isGimbalResverse.Pitch = DualStage::PITCH_REVERSE;
    isGimbalResverse.FirstYaw = DualStage::FIRST_YAW_REVERSE;
    isGimbalResverse.SecondYaw = DualStage::SECOND_YAW_REVERSE;

    InitConfig_s pitchDM4310Config = {
        .pComHandle = reinterpret_cast<uint32_t *>(&HCAN2),
        .comType = ComType_e::CAN,
        .motorType = SupportMotor_e::UNKNOWN,
        .offsetId = 1,
        .txFreq = PITCH_MOTOR_FREQ,
        .isReverse = PITCH_REVERSE,
    };
    motors_.Pitch = new DMMOTOR::DM4310("Pitch", pitchDM4310Config, DMMOTOR::WorkMode_e::UNKNOWN);
    motors_.Pitch->setZeroAng(PITCH_LAUNCH_ANG);
    LOG::info("Gimbal", "Pitch zeros has set to %f", motors_.Pitch->data().zeroAng);

    InitConfig_s firstYawDM4310Config = {
        .pComHandle = reinterpret_cast<uint32_t *>(&HCAN3),
        .comType = ComType_e::NONE,
        .motorType = SupportMotor_e::UNKNOWN,
        .offsetId = 2,
        .txFreq = FIRST_YAW_MOTOR_FREQ,
        .isReverse = FIRST_YAW_REVERSE,
    };
    motors_.FirstYaw = new DMMOTOR::DM4310("BigYaw", firstYawDM4310Config, DMMOTOR::WorkMode_e::UNKNOWN);
    motors_.FirstYaw->setZeroAng(FIRST_YAW_LAUNCH_ANG);
    LOG::info("Gimbal", "Big yaw zeros has set to %f", motors_.FirstYaw->data().zeroAng);

    InitConfig_s secondYawDM4310Config = {
        .pComHandle = reinterpret_cast<uint32_t *>(&HCAN2),
        .comType = ComType_e::NONE,
        .motorType = SupportMotor_e::UNKNOWN,
        .offsetId = 3,
        .txFreq = SECOND_YAW_MOTOR_FREQ,
        .isReverse = SECOND_YAW_REVERSE,
    };
    motors_.SecondYaw = new DMMOTOR::DM4310("SmallYaw", secondYawDM4310Config, DMMOTOR::WorkMode_e::UNKNOWN);
    motors_.SecondYaw->setZeroAng(SECOND_YAW_LAUNCH_ANG);

    LOG::info("Gimbal", "Small yaw zeros has set to %f", motors_.SecondYaw->data().zeroAng);

    this->stateFactory_.addState(FSMState_e::STOP, std::make_unique<StopState>(this));
    this->stateFactory_.init(this->stateFactory_.getNextState(FSMState_e::STOP));

}

void DualStage::stop()
{
    for (auto &i : motors_._) {
        i->cmd(PINYMOTOR::MotorCmdType_e::OFF);
    }
    for (auto &i : gimbalFilterOutput_._) {
        i->reset();
    }
}

void DualStage::enter()
{
    //TODO:
}

void DualStage::update(void *_param)
{
    //TODO:

    this->stateFactory_.update();
}

/**
 * @brief 缓启动专用
 * 
 * @param _pos 电机系角度
 * @param _step 缓启动步长
 * @param _angPID 
 * @param _velPID 
 */
void DualStage::ctrlSmooth(DualStage_u<float> _pos, DualStage_u<float> _step, DualStage_u<PositionalPid *> _angPID,
                           DualStage_u<PositionalPid *> _velPID)
{
}

// TODO: implement imu-frame to motor-frame angle conversion
DualStage_u<float> DualStage::ctrl(DualStage_u<float> _insPos)
{
    return {};
}

/**
 * @brief 云台的电机控制接口，输入电机系角度，输出电机转矩
 * TODO: implement motor control (angle error -> PID -> torque)
 *
 * @param _expMotorAng 电机系角度
 * @param _angPID
 * @param _velPID
 */
void DualStage::ctrlMotor(DualStage_u<float> _expMotorAng, DualStage_u<PositionalPid *> _angPID,
                          DualStage_u<PositionalPid *> _velPID)
{
}
