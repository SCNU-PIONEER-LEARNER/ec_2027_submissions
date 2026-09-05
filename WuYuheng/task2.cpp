#include <iostream>
#include <iterator>
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
    float average;
    float sum = 0;
    struct Student{
        string name;
        int id;
        float score;
    };
    Student *p = new Student[5];
    for(int i = 0;i<5;i++){
        cout <<"请输入第"<<i+1<<"位同学的名字"<<endl;
        cin >> p[i].name;
        p[i].id = i;
        cout <<"请输入第"<<i+1<<"位同学的成绩"<<endl;
        cin >> p[i].score;
        sum = sum + p[i].score;
    }
    average = sum/5;
    cout << "所有信息录入完毕，计算得到这些同学的平均分为" <<average<<endl;
    delete[] p;
    return 0;
}