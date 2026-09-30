#include<iostream>
using namespace std;
float gy(float m,float n, float x)
{
float result;
result=(x-m)/((n-m)/2)-1;
return result;    
}
int main()
{
while(true)
{
float m,n,x,y;
cout<<"Enter the value of m,n and x(m<x<n)"<<endl;
cin>>m>>n>>x;
y=gy(m,n,x);
cout<<y<<endl;
}
}