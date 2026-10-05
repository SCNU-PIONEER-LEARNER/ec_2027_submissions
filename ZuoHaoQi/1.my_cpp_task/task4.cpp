#include <iostream>
#include <cmath>
using namespace std;

const double PI = 3.14159265;

float convertX(float x, float y, float theta) {
    return x * cos(theta) - y * sin(theta);
}

float convertY(float x, float y, float theta) {
    return x * sin(theta) + y * cos(theta);
}

int main() {
    float x = 0.0f, y = 0.0f, theta = 0.0f;
    cout << "输入 x y 旋转角度(度): ";
    cin >> x >> y >> theta;

    theta = theta * PI / 180.0;

    float a = convertX(x, y, theta);
    float b = convertY(x, y, theta);

    if (fabs(a) < 1e-6) a = 0;
    if (fabs(b) < 1e-6) b = 0;

    cout << "新坐标: (" << a << ", " << b << ")" << endl;
    return 0;
}
