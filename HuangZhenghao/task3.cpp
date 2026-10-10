#include <iostream>

float rangemap(float m,float n,float x){
if(m==n){
    return 0;
}
float y=-1.0f+(x-m)*2.0f/(n-m);
return y;}    
int main() {
    float m=1.0f;
    float n=5.0f;
    float x=4.0f;
    
    float result=rangemap(m,n,x);
    

    
    std::cout <<"归一化后y值为:"<<result<< std::endl;
    return 0;
}
