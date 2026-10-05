#include <iostream>
#include <cmath>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#endif

const float PI = 3.14159265358979f;

// 绕原点逆时针旋转 theta（弧度）后的新 x 坐标
// 推导：a = x·cos θ - y·sin θ
float convertX(float x, float y, float theta) {
    return x * cos(theta) - y * sin(theta);
}

// 绕原点逆时针旋转 theta（弧度）后的新 y 坐标
// 推导：b = x·sin θ + y·cos θ
float convertY(float x, float y, float theta) {
    return x * sin(theta) + y * cos(theta);
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);  // 让 Windows 终端正确显示中文
#endif
    float x, y, alpha, thetaDeg;
    std::cout << "请输入点坐标 x y（空格分隔）: ";
    std::cin >> x >> y;
    std::cout << "请输入线段与 x 轴夹角 α（度）: ";
    std::cin >> alpha;
    std::cout << "请输入逆时针旋转角 θ（度）: ";
    std::cin >> thetaDeg;

    // 角度转弧度（C++ 的三角函数用弧度）
    float rad = thetaDeg * PI / 180.0f;

    float a = convertX(x, y, rad);
    float b = convertY(x, y, rad);

    std::cout << "旋转后的新坐标: a = " << a << ", b = " << b << std::endl;

    system("pause");  // 按任意键才关闭窗口，避免一闪而过
    return 0;
}
