#include<iostream>

using namespace std;

float rangemap(float m,float n,float x){
    float y = ((x-m)/(n-m)) * 2 - 1;
    return y;
}

int main(){
    float m,n,x;
    cin>>m>>n>>x;
    cout<<rangemap(m,n,x)<<endl;
    return 0;
}