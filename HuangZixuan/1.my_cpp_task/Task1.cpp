#include<iostream>
#include<vector>

using namespace std;

int main(){
    long long n;
    cin>>n;
    vector<long long> Cn;
    for(long long i{1};i*i<=n;++i){
        if(n%i==0){
            Cn.push_back(i);
        }
    }
    cout<<Cn.size()<<endl;
    for(long long i{0};i<Cn.size();++i){
        cout<<Cn[i]<<endl;
    }
    return 0;
}