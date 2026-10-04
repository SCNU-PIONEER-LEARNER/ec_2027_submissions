#include <iostream>
using namespace std;
float rangemap(float n, float m,float x){
    float result = 2*(x-n)/(m-n)-1;
    return result;
}
int main(){
    system("chcp 65001");
    float n,m,x;
    cout<<"请输入第一个数：";
    cin>>n;
    cout<<"请输入第二个数：";
    cin>>m;
    cout<<"请输入第三个数：";
    cin>>x;
    float result = rangemap(n, m, x);
    cout<<"结果为："<<result<<endl;
    return 0;
}