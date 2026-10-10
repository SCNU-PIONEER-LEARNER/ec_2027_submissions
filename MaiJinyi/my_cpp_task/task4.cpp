#include <iostream>
#include <cmath>
using namespace std;
float pi=3.14159;
float new_theta(float theta){
    return theta/180.0*pi;
}
float convertX(float x, float y,float theta){
    return x*cos(new_theta(theta))-y*sin(new_theta(theta));
}
float convertY(float x,float y, float theta){
    return x*sin(new_theta(theta))+y*cos(new_theta(theta));
}
int main(){
    float x;
    float y;
    float theta;
    cout<<"the original coordinates of point z are (x,y),now please enter x , y and theta in order"<<endl;
    cin>>x>>y>>theta;
    float a=convertX(x,y,theta);
    float b=convertY(x,y,theta);
    cout<<"the new coordinates of this point are:("<<a<<","<<b<<")"<<endl;
    return 0;
}