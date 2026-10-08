#include <iostream>
#include <cmath>
using namespace std;
float convertX(float x,float y,float theta){
float a;
a=x*cos(theta)-y*sin(theta);
return a;
}
float convertY(float x,float y,float theta){
float b;
b=x*sin(theta)+y*cos(theta);
return b;
}
int main()
{
float x,y,alpha,theta;
float a,b;
cout<<"input x y theta:";
cin>>x>>y>>theta;
a=convertX(x,y,theta);b=convertY(x,y,theta);
cout<<"a="<<a<<" b="<<b<<endl;
return 0;
}
