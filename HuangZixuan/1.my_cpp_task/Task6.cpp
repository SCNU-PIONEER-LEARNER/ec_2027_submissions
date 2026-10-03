#include<iostream>

using namespace std;

class Geometry{
public:
    virtual double volume() const=0;
    virtual double surfaceArea() const=0;

    virtual ~Geometry(){}
};

class Cube:public Geometry{
private:
    double side;

public:
    Cube(double s) : side(s) {}

    double volume() const override {
        return side * side * side;
    }

    double surfaceArea() const override {
        return 6 * side * side;
    }
};

class Sphere:public Geometry{
private:
    double radius;

public:
    Sphere(double r) : radius(r) {}

    double volume() const override {
        return (4.0/3.0) * 3.14159 * radius * radius * radius;
    }

    double surfaceArea() const override {
        return 4 * 3.14159 * radius * radius;
    }
};

int main(){
    Cube cube(3.0);
    Sphere sphere(2.0);
    cout << "Cube Volume: " << cube.volume() << endl;
    cout << "Cube Surface Area: " << cube.surfaceArea() << endl;
    cout << "Sphere Volume: " << sphere.volume() << endl;
    cout << "Sphere Surface Area: " << sphere.surfaceArea() << endl;
    return 0;
}