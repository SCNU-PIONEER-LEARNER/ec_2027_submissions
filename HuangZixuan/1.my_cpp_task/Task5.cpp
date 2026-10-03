#include<iostream>
#include<vector>
#include<bitset>
#include<string>

using namespace std;

int count(int m,int n,string s){
    int result{0};
    int num{1};
    for(int i{m};i<=n;i++){
        if(s[7-i]=='1'){
            result+=num;
        }
        num*=2;
    }
    return result;
}

int main(){
    int m,n;
    vector<string> s;
    for(int i{0};i<4;i++){
        string str;
        cin>>str;  // 输入十六进制
        int value = stoi(str, nullptr, 16);  // 转成整数
        // cout<<bitset<8>(value).to_string()<<endl;
        s.push_back(bitset<8>(value).to_string());
    }
    int id=count(0,7,s[0]),
    vel=count(0,3,s[1]),
    acc=count(4,7,s[1]),
    temp=count(0,5,s[2]),
    torque_High=count(6,7,s[2]),
    torque_Low=count(0,3,s[3]),
    voltage=count(4,7,s[3]);

    cout<<id<<" "<<vel<<" "<<acc<<" "<<temp<<" "<<torque_High<<" "<<torque_Low<<" "<<voltage;

    return 0;
}