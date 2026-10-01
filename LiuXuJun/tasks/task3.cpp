#include"iostream"
#include"windows.h"
using namespace std;
float hanshu1 (float m,float n,float x){
    float a=n-m;
    float b=x-m;
    float c=2*b/a;
    float d=c-1;
    return d;
}
int main(){
    SetConsoleOutputCP(CP_UTF8);
    float m,n,x;
    cout<<"请输入m n x，满足m<x<n:"<<endl;
    cin>>m>>n>>x;

    float y=hanshu1(m,n,x);

    cout<<"y值为："<<y<<endl;
    return 0;

}