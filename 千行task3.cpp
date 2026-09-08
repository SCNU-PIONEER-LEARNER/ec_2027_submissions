#include <iostream>
using namespace std;

float rangemap(float m,float n,float x){
	float y=-1+2*((x-m)/(n-m));
	return y;
}
int main(){
	float m1,n1,x1;
	cin>>m1>>n1>>x1;
	float y=rangemap(m1,n1,x1);
	cout<<y;
	return 0;
}
