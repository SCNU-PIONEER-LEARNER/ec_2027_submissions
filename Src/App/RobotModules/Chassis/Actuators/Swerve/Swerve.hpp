#pragma once
#include "main.h"
#include "Soc.hpp"
#include "MsgImpl.hpp"
#include "Chassis.hpp"
#include "IMotor.hpp"
#include "INS.hpp"
#include "LPF.hpp"
#include "PidBasic.hpp"

namespace CHASSIS {

template <typename T> union Swerve_u {
    struct {
        T RF;
        T LF;
        T LB;
        T RB;
    };
    T _[4];
};

union Speed_u {
    struct {
        float vx; // m/s
        float vy; // m/s
        float wz; // rad/s
    };
    float _[3];
};


//喵板
class Swerve : public Chassis {
public:
    static constexpr float W_FRONT_REAR = 0.36f; //前后轮距/m
    static constexpr float W_LEFT_RIGHT = 0.36f; //左右轮距
    static constexpr float W_DIAMETER = 0.116f;  //轮子直径
    static constexpr float W_RADIUS = (W_DIAMETER / 2.0f);
    static constexpr float W_CIRCUMFERENCE = (M_PI * W_DIAMETER);
    static constexpr float CAR_RADIUS = 0.25456f; //车体旋转半径

    static constexpr float MAX_VX_SPEED = 2.f;
    static constexpr float MAX_VY_SPEED = 2.f;
    static constexpr float MAX_WZ_SPEED = 10.f;

    static constexpr float STEER_MOTOR_FREQ = 500.f;
    static constexpr float WHEEL_MOTOR_FREQ = 500.f;

    Swerve();

    void stop();
    void enter();
    void update(void *_param);
    void ctrl(const Speed_u &_refSpeed);

protected:
    void getSuperCapData();

    Speed_u forward(const Swerve_u<float> &_wSpeed);
    Swerve_u<float> reverse(const Speed_u &_speed);
    void motorCtrl(Swerve_u<float> _sTorq, Swerve_u<float> _wTorq);

private:
    Subscriber<INS_SYS::INSData_s> *insSub_;
    INS_SYS::INSData_s insData_ = {};

    Swerve_u<PINYMOTOR::IMotor *> steerMotor_, wheelMotor_;

#if EXTENSION_SUPERCAP
    SuperCap cap_{ &HCAN2 };
#endif
    //当前车体速度
    Speed_u curSpeed_;
    //3508实际轮速,期望轮速rad/s
    Swerve_u<float> curWSpeed_, expWSpeed_;
    //舵轮 当前方向角,期望方向角,当前速度,当前扭矩
    Swerve_u<float> curDir, expDir, sCurVel, sCurTorq;
    //舵轮零位角度，用于回正方向,rad
    Swerve_u<float> steerZero = { { .RF = 5.0083f, .LF = 4.95766449f, .LB = 3.41889358f, .RB = 5.52453974f } };

    Swerve_u<canHandle *> steerCan = { { .RF = &HCAN2, .LF = &HCAN1, .LB = &HCAN1, .RB = &HCAN2 } },
                          wheelCan = { { .RF = &HCAN2, .LF = &HCAN1, .LB = &HCAN1, .RB = &HCAN2 } };

    Swerve_u<FILTER::LPF *> steerFilterInput_ = {}; //舵轮输入滤波器
    Swerve_u<PositionalPid *> steerPosPid_ = {}, steerVelPid_ = {}, wheelVelPid_ = {};
};

} // namespace CHASSIS
