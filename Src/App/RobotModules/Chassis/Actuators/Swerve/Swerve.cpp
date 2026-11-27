#include "Swerve.hpp"
#include "IMotor.hpp"
#include "MotorCommonMacros.hpp"
#include "Soc.hpp"
#include "sdkconfig.h"
#include "PidBasic.hpp"
#include <cstdint>
#include <cstring>
#include "GM6020.hpp"
#include "M3508.hpp"
#include "TopicRouter.hpp"
#include "./State/StopState.hpp"

extern canHandle HCAN1;
extern canHandle HCAN2;
extern canHandle HCAN3;

using namespace CHASSIS;
using namespace PINYMOTOR;

constexpr float F_PI = std::numbers::pi_v<float>;

//以下数组可以塞到类内。但不好用free master
//RF 0 ,LF 1 ,LB 2 ,RB 3
FILTER::LPF steerFilterInput[4]{
    FILTER::LPF(1000.f, 4.f),
    FILTER::LPF(1000.f, 4.f),
    FILTER::LPF(1000.f, 4.f),
    FILTER::LPF(1000.f, 4.f),
};

PositionalPid steerPosPid[4] = {
    PositionalPid(20.f, 15.f, 0.4f, 1.f / 1000.f, 10.f, 25.f, 0.f),
    PositionalPid(25.f, 20.f, 0.4f, 1.f / 1000.f, 10.f, 25.f, 0.f),
    PositionalPid(25.f, 15.f, 0.4f, 1.f / 1000.f, 10.f, 25.f, 0.f),
    PositionalPid(25.f, 15.f, 0.4f, 1.f / 1000.f, 10.f, 25.f, 0.f),
};
PositionalPid steerVelPid[4] = {
    PositionalPid(0.2f, 0.0f, 0.0f, 1.f / 1000.f, 0.3f, 0.8f, 0.f),
    PositionalPid(0.2f, 0.0f, 0.0f, 1.f / 1000.f, 0.3f, 0.8f, 0.f),
    PositionalPid(0.2f, 0.0f, 0.0f, 1.f / 1000.f, 0.3f, 0.8f, 0.f),
    PositionalPid(0.2f, 0.0f, 0.0f, 1.f / 1000.f, 0.3f, 0.8f, 0.f),
};

PositionalPid wheelVelPid[4] = {
    PositionalPid(0.45f, 0.05f, 0.001f, 1.f / 1000.f, 2.f, 4.f, 0.1f),
    PositionalPid(0.45f, 0.05f, 0.001f, 1.f / 1000.f, 2.f, 4.f, 0.1f),
    PositionalPid(0.45f, 0.05f, 0.001f, 1.f / 1000.f, 2.f, 4.f, 0.1f),
    PositionalPid(0.45f, 0.05f, 0.001f, 1.f / 1000.f, 2.f, 4.f, 0.1f),
};

Swerve::Swerve()
{

    for (uint8_t i = 1; i <= 4; i++) {
        steerFilterInput_._[i - 1] = &steerFilterInput[i - 1];
        steerPosPid_._[i - 1] = &steerPosPid[i - 1];
        steerVelPid_._[i - 1] = &steerVelPid[i - 1];

        wheelVelPid_._[i - 1] = &wheelVelPid[i - 1];

        InitConfig_s gm6020Config = {
            .pComHandle = reinterpret_cast<uint32_t *>(steerCan._[i - 1]),
            .comType = ComType_e::NONE,
            .motorType = SupportMotor_e::UNKNOWN,
            .offsetId = i,
            .txFreq = STEER_MOTOR_FREQ,
            .isReverse = true,
        };
        steerMotor_._[i - 1] = new DJIMOTOR::GM6020("GM6020", gm6020Config, DJIMOTOR::WorkMode_e::UNKNOWN);
        steerMotor_._[i - 1]->setZeroAng(steerZero._[i - 1]);

        InitConfig_s m3508Config = {
            .pComHandle = reinterpret_cast<uint32_t *>(wheelCan._[i - 1]),
            .comType = ComType_e::NONE,
            .motorType = SupportMotor_e::UNKNOWN,
            .offsetId = i,
            .txFreq = WHEEL_MOTOR_FREQ,
            .isReverse = true,
        };
        wheelMotor_._[i - 1] = new DJIMOTOR::M3508("M3508", m3508Config, DJIMOTOR::WorkMode_e::UNKNOWN);
        //TODO: M3508 overrideReductionRatio   XROLL_GEARBOX_RR
       
    }

    this->stateFactory_.addState(FSMState_e::STOP, std::make_unique<StopState>(this));
    this->stateFactory_.init(this->stateFactory_.getNextState(FSMState_e::STOP));
}

void Swerve::stop()
{
    for (uint8_t i = 0; i < 4; i++) {
        steerMotor_._[i]->cmd(MotorCmdType_e::OFF);
        wheelMotor_._[i]->cmd(MotorCmdType_e::OFF);
    }
}

void Swerve::enter()
{
    //TODO:
}

void Swerve::update(void *_param)
{
  //TODO:
    
    this->stateFactory_.update();
}

/**
 * @brief 轮速得车速
 * @see 模型：https://www.zxytql.top/algorithm/Swerveing_wheel
 * @param _wSpeed 
 * @return Speed_u 
 */
Speed_u Swerve::forward(const Swerve_u<float> &_wSpeed)
{ 
    Speed_u speed;
    ///<这些数组索引与模型对应即,0:LF,1:LB,2:RB,3:RF
    float wheelComVel[4], sinAng[4], cosAng[4];

    float rX = W_FRONT_REAR / 2.f; ///<前后轮距的一半
    float rY = W_LEFT_RIGHT / 2.f; ///<左右轮距的一半
    float carRadius = sqrtf((rX * rX) + (rY * rY));
    // TODO: 
    return speed;
}
/**
 * @brief 车速得轮速
 * @see 模型：https://www.zxytql.top/Algorithm/Steering_Wheel
 * @param _speed 
 * @return WheelsSpeed_u 
 */

Swerve_u<float> Swerve::reverse(const Speed_u &_speed)
{
    Swerve_u<float> refWheels;
    ///<这些数组索引与模型对应即,0:LF,1:LB,2:RB,3:RF
    float wheelVx[4], wheelVy[4], wheelComVel[4];

    float rX = W_FRONT_REAR / 2.f; ///<前后轮距的一半
    float rY = W_LEFT_RIGHT / 2.f; ///<左右轮距的一半

// TODO: 
    return refWheels;
}

void Swerve::ctrl(const Speed_u &_refSpeed)
{
    Swerve_u<float> tempDir, sDel, sExpVel, sExpTorq, wExpTorq, refWSpeed = {};
    float vx = std::fmax(-MAX_VX_SPEED, std::fmin(_refSpeed.vx, MAX_VX_SPEED));
    float vy = std::fmax(-MAX_VY_SPEED, std::fmin(_refSpeed.vy, MAX_VY_SPEED));
    float wz = std::fmax(-MAX_WZ_SPEED, std::fmin(_refSpeed.wz, MAX_WZ_SPEED));

    // TODO: 

}

void Swerve::motorCtrl(Swerve_u<float> _sTorq, Swerve_u<float> _wTorq)
{
    for (uint8_t i = 0; i < 4; i++) {
        steerMotor_._[i]->cmdTorq(_sTorq._[i]);
        wheelMotor_._[i]->cmdTorq(_wTorq._[i]);
    }
}

