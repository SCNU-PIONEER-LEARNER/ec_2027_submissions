#include <iostream>
using namespace std;
const double PI=3.1415926;
class Geometry
{
public:
virtual double volume()=0;
virtual double surfaceArea()=0;
virtual~Geometry(){};
};
class Square:public Geometry
{
private:
double side;
public:
Square(double s)
  {
    side=s;
  }
double volume()
  {
    return side*side*side;
  }
double surfaceArea()
  {
    return 6*side*side;
  }
};
class Spherome:public Geometry
{
private:
double radius;
public:
Spherome(double r)
  {
    radius=r;
  }
double volume()
  {
    return (4.0/3.0)*PI*radius*radius*radius;
  }
double surfaceArea()
  {
    return 4*PI*radius*radius;
  }
};
int main()
{
    Square box(3.0);
    Spherome ball(2.0);
    cout<<"正方体:体积="<<box.volume()
        <<"表面积="<<box.surfaceArea()<<endl;
     cout<<"球:体积="<<ball.volume()
        <<"表面积="<<ball.surfaceArea()<<endl;
            
    return 0;
}