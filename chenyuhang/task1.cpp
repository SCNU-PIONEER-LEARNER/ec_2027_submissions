#include<iostream>
using namespace std;
int main(){
    cout<<"请输入一个正整数"<<endl;
    int a;
    cin>>a;
    int i,j;
    int x=0;
    cout<<"其中质数含有："<<endl;
    for(i=2;i<=a;i++){
        bool shu= true;
        for(j=2;j<i;j++){
            if(i%j==0){
                shu= false;
                break;
            }

        }
       if(shu){
        cout<<i<<endl;
        x+=1;
       }
    }
cout<<"总共有"<<x<<"个质数";



return 0;
}