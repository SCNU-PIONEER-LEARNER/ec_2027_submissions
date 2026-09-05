#include <iostream>
#include <string>
#ifdef _WIN32
#include <windows.h>
#endif
using namespace std;
int main() {
#ifdef _WIN32
    // 设置控制台按 UTF-8 解读输出,解决中文乱码(仅 Windows 生效,不影响 Linux)
    SetConsoleOutputCP(CP_UTF8);
#endif
    int num,i=2,a=2,value=0; 
    cout<<"请输入一个整数"<<endl;
    cin >> num;
    cout<<"1到n之间所有的质数有：";
    while (i<=num){
        while(a<=num){
            if(a==i){
                value++;
                cout<< i <<',';
                a=2;
                break;
            }else if(a!=i && i%a==0){
                a=2;
                break;
            }else{
                a++;
            }
        }
        i++;
    }
    cout<<"合计有"<< value <<"个"<<endl;
    return 0;
}