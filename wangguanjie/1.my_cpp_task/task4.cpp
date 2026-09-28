#include <iostream>
#include <cmath>
using namespace std;
const float PI=3.1415926f;
float convertX(float x,float y,float theta)
{
    float rad=theta*PI/180.0f;
    float a=x*cos(rad)-y*sin(rad);
    return a;
}
float convertY(float x,float y,float theta)
{
    float rad=theta*PI/180.0f;
    float b=x*sin(rad)+y*cos(rad);
    return b;
}
int main()
{
    float a,b,x,y,theta;
    cin>>x>>y>>theta;
    a=convertX(x,y,theta);
    b=convertY(x,y,theta);
    cout<<"a="<< a <<" b="<< b <<endl;
    return 0;
}