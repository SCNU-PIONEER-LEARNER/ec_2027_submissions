#include <iostream>
using namespace std;
float rangemap(float m,float n,float x){
    float bi = (x-m)/(n-m);
    float zhi = -1+2*bi ;
    return zhi;
}
int main(){
    cout<<"请输入三个数，且前两个数为初始区间起止值"<<endl;
    float a,b,c;
    cin>>a>>b>>c;
    cout<<"归一化后为"<<rangemap(a,b,c);
    return 0;
}