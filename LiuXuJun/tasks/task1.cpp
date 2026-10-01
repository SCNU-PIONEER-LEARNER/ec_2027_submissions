#include"iostream"
#include"windows.h"
using namespace std;
int main()
{
SetConsoleOutputCP(CP_UTF8);
int n;
cout<<"请输入：";
cin>>n;
int shuliang=0;
for(int i=2;i<=n;i++){
    bool zhishu=true;
    for(int j=2;j<i;j++){
    if(i%j==0){
        zhishu=false;
        break;
    }
}
if(zhishu){
    cout<<i<<",";
    shuliang++;
    
}
    
}cout<<"数量是"<<shuliang;
    return 0;
}