#include"iostream"
#include"cmath"
using namespace std;
const float PI=acos(-1);
float hanshu1(float x,float y,float jiaodu){
    float hudu=jiaodu*PI/180;
    return x*cos(hudu)-y*sin(hudu);
}
float hanshu2(float x,float y,float jiaodu){
    float hudu=jiaodu*PI/180;
    return x*sin(hudu)+y*cos(hudu);
}

int main(){
    float x,y,jiaodu;
    cout<<"请输入点（x,y）"<<endl;
    cin>>x>>y;
    cout<<"请输入旋转角度："<<endl;
    cin>>jiaodu;
    float a=hanshu1(x,y,jiaodu);
    float b=hanshu2(x,y,jiaodu);

    cout<<"旋转后坐标：("<<a<<","<<b<<")"<<endl;
    return 0;
}