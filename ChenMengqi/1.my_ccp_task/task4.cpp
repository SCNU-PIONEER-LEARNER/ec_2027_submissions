#define _USE_MATH_DEFINES
#include<cmath>
#include<iostream>
using namespace std;
float hs(float x,float y)
{
return x*x+y*y;
}
int main()
{
while(true)
{
float a,b,x,y,alpha,theta;
cout<<"Please x: y: theta:"<<endl;
if(!(cin>>x>>y>>theta))
{
break;
}
float l=hs(x,y);
float L=sqrt(l);
alpha=atan2(y,x);
a=L*cos(alpha+theta*M_PI/180);
b=L*sin(alpha+theta*M_PI/180);
cout<<"("<<a<<","<<b<<")"<<endl;
};
}