#include "PID.hpp"
#include <algorithm>
PositionalPid::PositionalPid(float _kp, float _ki, float _kd, float _dt, float _iMax, float _outMax)
        : kp(_kp), ki(_ki), kd(_kd), dt(_dt), iMax(_iMax), iOut(0.f), outMax(_outMax)
{
    err[0] = 0;
    err[1] = 0;
}

float PositionalPid::positionalCalc(float _ref, float _cur) {
    err[1]=err[0];// 旧误差挪到第二格
    err[0]=_ref-_cur ; //计算本拍误差
    iOut+=ki*err[0]*dt;
    iOut  = std::clamp(iOut, -iMax, iMax); //积分限幅，防止积分饱和，超调
    float out=kp*err[0]+iOut+kd*(err[0]-err[1])/dt;
    return std::clamp(out, -outMax, outMax);//输出限幅

}

IncrementalPid::IncrementalPid(float _kp, float _ki, float _kd, float _outMax)
        : kp(_kp), ki(_ki), kd(_kd), outMax(_outMax)
{
    err[0] = 0;
    err[1] = 0;
    err[2] = 0;
}

float IncrementalPid::incrementalCalc(float _ref, float _cur) {
   err[1]=err[0];// 旧误差挪到第二格
   err[2]=err[1];
    err[0]=_ref-_cur ; //计算本拍误差
    float delta = kp * (err[0] - err[1])                  // P 项：误差的“变化”
                + ki *  err[0]                            // I 项：当前误差
                + kd * (err[0] - 2.f * err[1] + err[2]);  // D 项：误差的二阶差分

    float Out = 0.f;                            // 累计输出（未在头文件声明成员）
    Out  += delta;                                     // ③ 累加成绝对量
    Out = std::clamp(Out, -outMax, outMax);           // ④ 限幅在累计值上
    return Out;




}
