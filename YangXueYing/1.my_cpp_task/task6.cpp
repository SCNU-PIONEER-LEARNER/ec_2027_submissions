#include <iostream>
using namespace std;

class Geometry {
public:
    virtual float volume() = 0;
    virtual float area() = 0;   
};

class Square : public Geometry {
private:
    float side;

public:
    Square(float s){
        side = s;
    }

    float volume() override {
        return side * side * side;
    }

    float area() override {
        return side * side * 6;
    }
};

class Spherome : Geometry{
private:
    float radius;

public:
Spherome(float r){
       radius = r;
    }
    float volume() override {
        return 4.0 * radius * radius * radius * 3.1415926 / 3.0;
    }

    float area() override {
        return 4.0 * radius * radius *3.1415926;
    }
};

int main(){
float s, r;

cout << "enter side :";
cin >> s;
Square sq(s);

cout << "enter radius :";
cin >> r;
Spherome sp(r);

cout << "the square_volume is " << sq.volume() << endl;
cout << "the square_area is " << sq.area() << endl;

cout << "the Spherome_volume is " << sp.volume() << endl;
cout << "the Spherome_area is" << sp.area() << endl;
}