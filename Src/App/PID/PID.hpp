#pragma once

class PositionalPid{
    public:
        PositionalPid(float _kp, float _ki, float _kd,float _dt,float _iMax,float _outMax);
        float positionalCalc(float _ref,float _cur);
        
    private:
        float err[2];
        float kp;
        float ki;
        float iOut;//注意：iOut是积分环节的输出，需要限幅
        float kd;
        float dt;
        float iMax;
        float outMax;
};

class IncrementalPid{
    public:
        IncrementalPid(float _kp, float _ki, float _kd,float _outMax);
        float incrementalCalc(float _ref,float _cur);
    private:
        float err[3];
        float kp;
        float ki;
        float kd;
        float outMax;
};