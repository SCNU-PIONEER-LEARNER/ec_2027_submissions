#include <iostream>
#include <iterator>
#ifdef _WIN32
#include <windows.h>
#endif
using namespace std;
float rangemap(float m,float n,float x){
    float scale,y;
    scale=(x-m)/(n-m);
    y=(-1)+2*scale;
    return y;
};
int main() {
#ifdef _WIN32
    // 设置控制台按 UTF-8 解读输出,解决中文乱码(仅 Windows 生效,不影响 Linux)
    SetConsoleOutputCP(CP_UTF8);
#endif
    float m,n,x,y;
    do{
    cout<<"请输入左区间m"<<endl;
    cin>>m;
    cout<<"请输入右区间n,注意确保n>m"<<endl;
    cin>>n;
    }while(m>=n);
    cout<<"请输入区间中的值x，注意确保m<x<n"<<endl;
    cin>>x;
    while(x<=m||x>=n){
        cout<<"输入的x不在区间范围内，请重新输入"<<endl;
        cin>>x;
    }
    y=rangemap(m, n, x);
    cout<<"x归一化的后的值y="<<y<<endl;
    return 0;
}