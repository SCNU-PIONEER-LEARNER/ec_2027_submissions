#include <iostream>
using namespace std;
int main()
{
while(true)
{
int a,b,m,n;
b=0;
cout<<"Please enter an integer"<<endl;
cin>>n;
for(a=2;a<=n;a++)
{
bool zs=true; 
for(m=2;m<a;m++)
{
 if(a%m==0)
{
  zs=false;
  break;
}
}
if(zs)
{
b=b+1;
cout<<a<<endl;    
}
}
cout<<"Number of primes:"<<b<<endl;
}
}
