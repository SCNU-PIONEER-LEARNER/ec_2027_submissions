#include <iostream>
#include <cmath>
using namespace std;

const double PI = 3.14159265;

class Geometry {
public:
    virtual double volume() = 0;
    virtual double area()   = 0;
};

class Cube : public Geometry {
private:
    double side;
public:
    Cube(double s) {
        side = s;
    }
    double volume() { return side * side * side; }
    double area()   { return 6 * side * side; }
};

class Sphere : public Geometry {
private:
    double r;
public:
    Sphere(double radius) {
        r = radius;
    }
    double volume() { return 4.0 / 3.0 * PI * r * r * r; }
    double area()   { return 4 * PI * r * r; }
};

int main() {
    double side = 0, radius = 0;

    cout << "输入正方体边长: ";
    cin >> side;
    cout << "输入球的半径:   ";
    cin >> radius;

    Cube   c(side);
    Sphere s(radius);

    cout << endl;
    cout << "正方体(边长 " << side << "): 体积=" << c.volume()
         << "  表面积=" << c.area() << endl;
    cout << "球(半径 " << radius << "): 体积=" << s.volume()
         << "  表面积=" << s.area() << endl;

    Geometry *p = &c;
    cout << endl << "用基类指针调用: volume = " << p->volume() << endl;

    return 0;
}
