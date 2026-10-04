#include <iostream>
using namespace std;
int main(){
    system("chcp 65001");
    int n;
    cout<<"请输入一个整数n"<<endl;
    cin>>n;
    if(n<2){
        cout<<"1到"<<n<<"之间有0个质数"<<endl;
    }
    else if (n==2){
        cout<<"1到"<<n<<"之间有1个质数"<<endl;
    }
    else{
       int a=3;
       int num=1;
       cout<<"1到"<<n<<"之间有质数：2,";
       
       while(a<=n){
         int b=2;
         bool i=true;
         while(b<a){
             if(a%b==0){
                 i=false;
                 break;
             }
             b++;   
        }
        if(i){ 
             num++;
             cout<<a<<"，";
         }
         a++;
       }
        cout<<"1到"<<n<<"之间有"<<num<<"个质数"<<endl;
    }

return 0;}
