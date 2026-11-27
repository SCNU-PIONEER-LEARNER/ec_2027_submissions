#pragma once

#include "Chassis.hpp"
#include "Gimbal.hpp"
#include "Bsp_can.hpp"
#include "IMotor.hpp"
#include "MsgImpl.hpp"
#include "LPF.hpp"
#include "MotorCommonMacros.hpp"
#include "PidBasic.hpp"
#include "SuperCap.hpp"

namespace GIMBAL {

template <typename T> union DualStage_u {
    struct {
        T Pitch;
        T FirstYaw;  //大yaw
        T SecondYaw; //小yaw
    };
    T _[3];
};


/**
 * @brief 双级yaw轴,大小yaw.
 * @note 当前版本imu装在小yaw上,但是控制的是大yaw
 * @note 尝试大yaw跟小yaw，小yaw跟imu
 */
class DualStage : public Gimbal {
public:
    static constexpr float PITCH_LAUNCH_ANG = 1.16512966f;
    static constexpr float FIRST_YAW_LAUNCH_ANG = 0.883578777f;
    static constexpr float SECOND_YAW_LAUNCH_ANG = 3.38531876f;

    static constexpr float PITCH_MOTOR_FREQ = 1000.f;
    static constexpr float FIRST_YAW_MOTOR_FREQ = 1000.f;
    static constexpr float SECOND_YAW_MOTOR_FREQ = 1000.f;

    // "REVERSE flag" is true when imu and motor have different direction
    static constexpr bool PITCH_REVERSE = true;
    static constexpr bool FIRST_YAW_REVERSE = false;
    static constexpr bool SECOND_YAW_REVERSE = true;

    static constexpr float MAX_BYAW_SPEED = 5.f; //4340额定转速36rpm,实测5rad/s还行
    static constexpr float MAX_BYAW_FOLLOW_SPEED = 10.f;
    //imu
    static constexpr float IMU_PITCH_MAX_ANG = 0.53f;  //
    static constexpr float IMU_PITCH_MIN_ANG = -0.58f; //
    //motor
    static constexpr float SECOND_YAW_MAX_ANG = 0.22f; // 13°
    static constexpr float SECOND_YAW_MIN_ANG = -0.22f;
 
    DualStage();

    DualStage_u<bool> isGimbalResverse;
    DualStage_u<float> curImuAng_, expImuAng_;     //-pi~pi
    DualStage_u<float> curMotorAng_, expMotorAng_; //0~2pi

    void stop();
    void enter();
    void update(void *_param);

    DualStage_u<float> ctrl(DualStage_u<float> _insPos);
    void ctrlMotor(DualStage_u<float> _expMotorAng, DualStage_u<PositionalPid *> _angPID,
                   DualStage_u<PositionalPid *> _velPID);
    void ctrlSmooth(DualStage_u<float> _pos, DualStage_u<float> _step, DualStage_u<PositionalPid *> _angPID,
                    DualStage_u<PositionalPid *> _velPID);

private:

    DualStage_u<FILTER::LPF *> gimbalFilterOutput_; //输出滤波
    DualStage_u<PINYMOTOR::IMotor *> motors_;       //电机
};

} // namespace GIMBAL
