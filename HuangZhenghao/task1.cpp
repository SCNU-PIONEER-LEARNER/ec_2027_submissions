#include <iostream>

int main() {
    int n;
    std::cout<<"请输入整数n:";
    std::cin>>n;
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
        if(isPrime)
        {
            std::cout<<i<<" ";
            count++;
        }
    }
    std::cout <<"\n质数总共有:"<<count<< std::endl;
    return 0;
}