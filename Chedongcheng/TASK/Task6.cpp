#include <iostream>
#include <cmath>
using namespace std;
const double PI =acos(-1);
class Geometry {
public:
    virtual double volume() = 0;        
    virtual double surfaceArea() = 0;   
    virtual ~Geometry() {}             
};
class Cube : public Geometry {
private:
    double side;                        
public:
    Cube(double s) : side(s) {}         

    double volume() override {          
        return side * side * side;
    }
    double surfaceArea() override {     
        return 6 * side * side;
    }
};
class Sphere : public Geometry {
private:
    double radius;                      
public:
    Sphere(double r) : radius(r) {}     

    double volume() override {         
        return 4.0 / 3.0 * PI * radius * radius * radius;
    }
    double surfaceArea() override {     
        return 4 * PI * radius * radius;
    }
};
int main() {
    system("chcp 65001");
    double size;
    double r;
    cout<<"请输入正方形的边长"<<endl;
    cin>>size;
    cout<<"请输入球的半径"<<endl;
    cin>>r;
    Cube c(size);          
    Sphere s(r);        
    cout << "正方体" << endl;
    cout << "体积 = " << c.volume() << endl;
    cout << "表面积 = " << c.surfaceArea() << endl;
    cout << endl << "球" << endl;
    cout << "体积 = " << s.volume() << endl;
    cout << "表面积 = " << s.surfaceArea() << endl;

    return 0;
}