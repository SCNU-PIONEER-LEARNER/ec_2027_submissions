#define _USE_MATH_DEFINES
#include<iostream>
#include<cmath>
using namespace std;
class Geometry 
{
public:
virtual float S()=0;
virtual float V()=0;
};
class ZFT:  public Geometry
{
private:
float bianchang;
public:
ZFT(float a):bianchang(a){};
float S() override
{
return 6*bianchang*bianchang;    
}
float V() override
{
 return bianchang*bianchang*bianchang;   
}
};
class QT:public Geometry
{
private:
float R;
public:
QT(float r):R(r){};
float S() override
{
return 4*M_PI*R*R;
};
float V() override
{
return M_PI*4/3*R*R*R;    
};
};
int main()
{
ZFT zft(10);
QT qt(10);
cout<<"正方体："<<endl;
cout<<"表面积："<<zft.S()<<endl<<"体积："<<zft.V()<<endl;
cout<<"球体:"<<endl;
cout<<"表面积："<<qt.S()<<endl<<"体积:"<<qt.V()<<endl;
}





