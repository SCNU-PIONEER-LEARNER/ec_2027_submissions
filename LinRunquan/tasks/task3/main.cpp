#include <iostream>
using namespace std;
float rangemap(float m,float n,float x)
{
    float y;
y=2*(x-m)/(n-m)-1;
    return y;
}
int main()
{
    float m,n,x;
cin>>m>>n>>x;
    float res;
    res=rangemap(m,n,x);
    cout<<res<<endl;
    return 0;
}
