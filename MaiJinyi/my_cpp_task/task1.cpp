#include <iostream>
using namespace std;
int main(){
    int n;
    cout<<"please enter the limit: ";
    cin>>n;
    int total=0;
    for(int a=1;a<=n;a++){
        int count=0;
        for(int b=1;b<=a;b++){
            if(a%b==0){
                count++;
            }
        }
        if(count==2){
            total++;
        cout<<a<<" is a prime number"<<endl;
    }
    }
    cout<<"total prime numbers are:"<<total<<endl;
    return 0;
}