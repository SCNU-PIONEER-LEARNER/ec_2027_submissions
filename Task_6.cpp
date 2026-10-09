#include <iostream>
using namespace std;
class Geometry {
public:
    virtual float getVolume() = 0; 
    virtual float getArea() = 0;   
    virtual ~Geometry() {}         
};
class Square : public Geometry {
private:
    float side; 

public:
     Square(float s) : side(s) {} 
    
    float getVolume() override { return side * side * side; }
    float getArea() override { return 6 * side * side; }
};

class Spherome : public Geometry {
private:
    float radius; 

public:

    Spherome(float r) : radius(r) {}
    
    float getVolume() override { return 4.0/3.0 * 3.14159 * radius * radius * radius; }
    float getArea() override { return 4 * 3.14159 * radius * radius; }
};

int main() {
  
    Square sq(2.0); 
    Spherome sp(3.0);
    
    cout << "Square Volume: " << sq.getVolume() << ", Area: " << sq.getArea() << endl;
    cout << "Spherome Volume: " << sp.getVolume() << ", Area: " << sp.getArea() << endl;
    
    return 0;
}