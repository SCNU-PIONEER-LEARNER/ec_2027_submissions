#include <iostream>
using namespace std;
int main()
{
    int n;
    cout<<"请输入数字n"<<endl;
    cin>>n;
    int count=0;
    for(int i=2;i<=n;i++)
    {
        bool isPrime=true;
        for(int m=2;m<i;m++)
        {
            if(i%m==0)
            {
                isPrime=false;
                break;
            }
        }
        if(isPrime==true)
        {
            cout<<i<<endl;
            count++;
        }
    }
    cout<<"一共有"<<count<<"个质数"<<endl;
    return 0;
}