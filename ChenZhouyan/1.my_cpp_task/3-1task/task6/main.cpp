#include <iostream>
#include <windows.h>

using namespace std;

const double PI = 3.14159;

// 几何体类：只定一个“标准”，具体怎么算交给子类
// 带 = 0 的函数叫纯虚函数，意思是自己不实现，子类必须实现
class Geometry {
public:
    virtual double volume() = 0;       // 求体积
    virtual double surfaceArea() = 0;  // 求表面积
    virtual ~Geometry() {}             // 基类的析构函数要写成 virtual，是好习惯
};

// 正方体，继承 Geometry
class Square : public Geometry {
private:
    double a;   // 边长，private 表示外面不能直接改
public:
    Square(double side) { a = side; }
    double volume() { return a * a * a; }         // 体积 = 边长立方
    double surfaceArea() { return 6 * a * a; }    // 表面积 = 6 个面
};

// 球，继承 Geometry
class Spherome : public Geometry {
private:
    double r;   // 半径
public:
    Spherome(double radius) { r = radius; }
    double volume() { return 4.0 / 3.0 * PI * r * r * r; }  // 球体积公式
    double surfaceArea() { return 4 * PI * r * r; }         // 球表面积公式
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
