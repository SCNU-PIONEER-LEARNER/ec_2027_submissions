#include <iostream>
using namespace std;
#include <cmath>
float a,b,x,y,alpha,theta,len,x2,y2;
const float huan=3.14/180;



float convertX(float x,float theta){
        x2=len*cos(alpha*huan+theta*huan);
        return x2;
    }
    
float convertY(float y,float theta){
        y2=len*sin(alpha*huan+theta*huan);
        return y2;
    }
    
    
int main(){
    cout<<"请依次输入坐标x,y以及初始角度，旋转角度"<<endl;
    cin>>x>>y>>theta>>alpha;
    len=sqrt(x*x+y*y);
    a=convertX(x,theta);
    b=convertY(y,theta);
    cout<<"旋转后坐标为"<<"("<<a<<","<<b<<")";
    return 0;
}