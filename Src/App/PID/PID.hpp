#pragma once

class PositionalPid{
    public:
        PositionalPid(float _kp, float _ki, float _kd,float _dt,float _iMax,float _outMax);
        float positionalCalc(float _ref,float _cur);//期望值，比较值,算控制量
        void reset() { err[0] = 0.f; err[1] = 0.f; iOut = 0.f; }
        //err[0]本拍误差，err[0] = _ref - _cur = 期望 - 实测，P 项 = kp × err[0]；就是这一次控制里“还差多少”。它每 1 ms 被重新算一次，是 P 项的直接来源：
        /*err[1]上一拍误差，err[1] = err[0]; // 先把旧的挪到第二格；err[0] = _ref - _cur; // 再算新的放第一格
        D 项（微分项）要的是误差的变化率：
        D 项 = kd × (err[0] - err[1]) / dt*/
        //iOut 积分累加值，iOut = iOut + ki × err[0]
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
//PositionalPid位置式
class IncrementalPid{
    public:
        IncrementalPid(float _kp, float _ki, float _kd,float _outMax);
        float incrementalCalc(float _ref,float _cur);
        void reset() { err[0] = 0.f; err[1] = 0.f; err[2] = 0.f; };
    private:
        float err[3];
        float kp;
        float ki;
        float kd;
        float outMax;
    //累计输出值，累计输出（增量式需要，位置式每拍独立算）
};//IncrementalPid增量式