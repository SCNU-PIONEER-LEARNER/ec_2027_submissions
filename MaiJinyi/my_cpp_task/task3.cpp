#include <iostream>
using namespace std;
float rangemap(float m,float n,float x){
    float y=(x-m)/(n-m)*2+(-1);
    return y;
}
int main(){
    float y=rangemap(1,5,4);
    cout<<"y="<<y<<endl;
    return 0;
}