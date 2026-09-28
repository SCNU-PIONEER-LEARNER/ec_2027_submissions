#include <iostream>
using namespace std;
float rangemap(float m,float n,float x)
{
    float t=(x-m)/(n-m);
    float y=2*t-1;
    return y;
}
int main()
{
    float m;
    cout<<"请输入数字m"<<endl;
    cin>>m;
    float n;
    cout<<"请输入数字n"<<endl;
    cin>>n;
    float x;
    cout<<"请输入数字x"<<endl;
    cin>>x;  
    float y=rangemap(m,n,x);
    cout<<y<<endl;
    return 0;
}