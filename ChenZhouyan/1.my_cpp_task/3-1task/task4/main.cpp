#include <iostream>
#include <cmath>
#include <windows.h>

using namespace std;

const float PI = 3.14159f;

// 绕原点逆时针转 theta 弧度后，点 (x, y) 的新横坐标
float convertX(float x, float y, float theta) {
    return x * cos(theta) - y * sin(theta);
}

// 绕原点逆时针转 theta 弧度后，点 (x, y) 的新纵坐标
float convertY(float x, float y, float theta) {
    return x * sin(theta) + y * cos(theta);
}

int main() {
    SetConsoleOutputCP(CP_UTF8);   // 让终端正常显示中文

    float x, y, alpha, theta;
    cout << "请输入点坐标 x y（空格分隔）: ";
    cin >> x >> y;
    cout << "请输入线段与 x 轴夹角 α（度）: ";
    cin >> alpha;
    cout << "请输入逆时针旋转角 θ（度）: ";
    cin >> theta;

    // C++ 的 sin/cos 用弧度，所以先把角度乘 PI 除 180 转成弧度
    float rad = theta * PI / 180.0f;

    float a = convertX(x, y, rad);
    float b = convertY(x, y, rad);

    cout << "旋转后的新坐标: a = " << a << ", b = " << b << endl;

    system("pause");   // 按任意键再关窗口
    return 0;
}
