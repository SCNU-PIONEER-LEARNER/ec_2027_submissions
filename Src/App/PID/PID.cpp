#include "PID.hpp"
#include <algorithm>
PositionalPid::PositionalPid(float _kp, float _ki, float _kd, float _dt, float _iMax, float _outMax)
        : kp(_kp), ki(_ki), kd(_kd), dt(_dt), iMax(_iMax), iOut(0.f), outMax(_outMax)
{
    err[0] = 0;
    err[1] = 0;
}

float PositionalPid::positionalCalc(float _ref, float _cur) {}

IncrementalPid::IncrementalPid(float _kp, float _ki, float _kd, float _outMax)
        : kp(_kp), ki(_ki), kd(_kd), outMax(_outMax)
{
    err[0] = 0;
    err[1] = 0;
    err[2] = 0;
}

float IncrementalPid::incrementalCalc(float _ref, float _cur) {}
