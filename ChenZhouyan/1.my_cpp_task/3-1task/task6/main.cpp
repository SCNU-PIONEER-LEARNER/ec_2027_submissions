#include <iostream>
#include <windows.h>

using namespace std;

const double PI = 3.14159;

// 基类：规定"所有几何体都能算体积和表面积"
// = 0 表示纯虚函数：基类自己不算，留给子类去实现
class Geometry {
public:
    virtual double volume() = 0;
    virtual double surfaceArea() = 0;
};

// 正方体，继承 Geometry
class Square : public Geometry {
public:
    double a;   // 边长
    Square(double side) { a = side; }
    double volume() { return a * a * a; }        // 体积 = 边长立方
    double surfaceArea() { return 6 * a * a; }   // 表面积 = 6 个面
};

// 球，继承 Geometry
class Spherome : public Geometry {
public:
    double r;   // 半径
    Spherome(double radius) { r = radius; }
    double volume() { return 4.0 / 3.0 * PI * r * r * r; }   // 球体积公式
    double surfaceArea() { return 4 * PI * r * r; }          // 球表面积公式
};

int main() {
    SetConsoleOutputCP(CP_UTF8);   // 让终端正常显示中文

    double side, radius;
    cout << "请输入正方体边长: ";
    cin >> side;
    cout << "请输入球半径: ";
    cin >> radius;

    Square sq(side);       // 建一个正方体对象
    Spherome sp(radius);   // 建一个球对象

    cout << "正方体 -> 体积 = " << sq.volume()
         << " , 表面积 = " << sq.surfaceArea() << endl;
    cout << "球     -> 体积 = " << sp.volume()
         << " , 表面积 = " << sp.surfaceArea() << endl;

    system("pause");   // 按任意键再关窗口
    return 0;
}
