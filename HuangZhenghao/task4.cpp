#include <iostream>
#include <cmath>
const double PI=3.14159265;
float a,b,x,y,alpha,theta;
float r;
float convertX(float x,float y,float theta){
    alpha=atan2(y,x);
a=r*cos(alpha+theta*PI/180.0f);
return a;    
}
float convertY(float x,float y,float theta){
    alpha=atan2(y,x);
b=r*sin(alpha+theta*PI/180.0f);
return b;    
}



int main() {
    std::cout<<"请输入点坐标x,y:";
    std::cin>>x>>y;
    r=sqrt(x*x+y*y);
    std::cout<<"请输入旋转的角度theta:"<<std::endl;
    std::cin>>theta;
    a=convertX(x,y,theta);
    b=convertY(x,y,theta);



    std::cout << "新的坐标为"<<a<<" "<<b<< std::endl;
    return 0;
}
