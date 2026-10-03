#include<iostream>
#include<cmath>

using namespace std;

void Rotate(float x,float y,float theta,float &x1,float &y1){
    x1 = x*cos(theta) - y*sin(theta);
    y1 = x*sin(theta) + y*cos(theta);
}

int main(){
    float x,y,theta;
    cin>>x>>y>>theta;
    float x1,y1;
    Rotate(x,y,theta,x1,y1);
    cout<<x1<<" "<<y1<<endl;
    return 0;
}