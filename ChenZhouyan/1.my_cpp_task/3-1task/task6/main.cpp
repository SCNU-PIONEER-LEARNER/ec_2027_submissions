#include <iostream>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#endif

const double PI = 3.14159265358979;

// 几何体抽象基类：只声明接口（纯虚函数），不能实例化
class Geometry {
public:
    virtual double volume() = 0;        // 纯虚函数：体积
    virtual double surfaceArea() = 0;   // 纯虚函数：表面积
    virtual ~Geometry() {}              // 虚析构函数（基类好习惯）
};

// 正方体：public 继承 Geometry
class Square : public Geometry {
private:
    double a;  // 边长——private，类外不能直接访问
public:
    Square(double side) : a(side) {}            // 构造函数，用初始化列表传边长
    double volume() override { return a * a * a; }          // 体积 = a³
    double surfaceArea() override { return 6.0 * a * a; }   // 表面积 = 6a²
};

// 球：public 继承 Geometry
class Spherome : public Geometry {
private:
    double r;  // 半径——private，类外不能直接访问
public:
    Spherome(double radius) : r(radius) {}                          // 构造函数传半径
    double volume() override { return 4.0 / 3.0 * PI * r * r * r; } // 体积 = 4/3 π r³
    double surfaceArea() override { return 4.0 * PI * r * r; }      // 表面积 = 4 π r²
};

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);  // 让 Windows 终端正确显示中文
#endif
    double side, radius;
    std::cout << "请输入正方体边长: ";
    std::cin >> side;
    std::cout << "请输入球半径: ";
    std::cin >> radius;

    Square sq(side);                 // 实例化正方体（构造函数传边长）
    Spherome sp(radius);             // 实例化球（构造函数传半径）

    std::cout << "正方体 -> 体积 = " << sq.volume()
              << " , 表面积 = " << sq.surfaceArea() << "\n";
    std::cout << "球     -> 体积 = " << sp.volume()
              << " , 表面积 = " << sp.surfaceArea() << "\n";

    system("pause");
    return 0;
}
