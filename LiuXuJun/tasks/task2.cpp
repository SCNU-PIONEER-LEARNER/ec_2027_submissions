#include"iostream"
#include"windows.h"
using namespace std;
struct Student{
        string name;
        int id;
        float score;
    };
int main(){
    SetConsoleOutputCP(CP_UTF8);
    Student stu[5];
    Student *p=stu;
    for (int i=0;i<5;i++){
        p->id=i+1;
        cout<<"请输入"<<i+1<<"的名字：";
        cin>>p->name;
        cout<<"请输入"<<i+1<<"的成绩：";
        cin>>p->score;
        p++;
    }
     p=stu;
    float sum=0;
    for(int i=0;i<5;i++){
        sum+=p->score;
        p++;
    }
   
    float average=sum/5;
    cout<<"平均成绩是："<<average<<endl;
    return 0;
}