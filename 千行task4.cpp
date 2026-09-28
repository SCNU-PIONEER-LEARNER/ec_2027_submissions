#include<iostream>
#include<cmath>
using namespace std;

float a,b,x,y,alpha,theta;
float convertX(float x,float theta);
float convertY(float y,float theta);

float convertX(float x,float theta)
{
	return x*cos(theta);
}

float convertY(float y,float theta)
{
	return y*sin(theta);
}


 
int main(){
	cin>>x>>y>>theta;
	float alpha=atan2(y,x);
	float m=alpha+theta;
	float r=sqrt(x*x+y*y);
	a=convertX(r,m);
	b=convertY(r,m);
	cout<<a<<","<<b<<endl;
	
	return 0;
} 
