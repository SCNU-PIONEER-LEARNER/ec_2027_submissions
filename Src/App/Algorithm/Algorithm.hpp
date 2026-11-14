#pragma once
#include <cmath>

float getMinorArc(float _end, float _start, float _range = 2 * M_PI) {
    float diff = std::fmodf(_end - _start, _range);
    if (diff > 0.5f * _range) { diff -= _range; } // 超过 +π：减一圈

    if (diff < -0.5f * _range) { diff += _range; }
    return diff;

}

static inline float rangeMap(float _scale, float _min, float _max) {
    const float range =_max-_min;//区间宽度
    float a =std::fmodf(_scale-_min,range); //卷进 (-range, range)
    if (a < 0.f) {
        a+=  range;} //负数补一圈，变成 [0, range)
    return _min + a;   //平移回目标区间


}
//_scale 是输入值