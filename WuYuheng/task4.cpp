#include <cmath>
#include <iostream>
#include <iterator>
#include "cmath"
const double PI =3.1415926535;
#ifdef _WIN32
#include <windows.h>
#endif
using namespace std;
int main() {
#ifdef _WIN32
    // 设置控制台按 UTF-8 解读输出,解决中文乱码(仅 Windows 生效,不影响 Linux)
    SetConsoleOutputCP(CP_UTF8);
#endif
    float x,y,a,b,theta;
    cout <<"请输入横坐标"<<endl;
    cin>>x;
    cout <<"请输入纵坐标"<<endl;
    cin>>y;
    cout <<"请输入旋转角度(°)"<<endl;
    cin>>theta;
    theta=theta*PI/180;
    a=x*cos(theta)-y*sin(theta);
    b=x*sin(theta)+y*cos(theta);
    cout << "新的横纵坐标分别为"<<a<<","<<b<<endl;
    return 0;
}