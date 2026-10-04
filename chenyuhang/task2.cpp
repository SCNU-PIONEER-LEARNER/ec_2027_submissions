#include <iostream>
using namespace std;
#include <string>
struct Student{
    string name;
    int id;
    float score;
};
int main(){
    Student stu [5];
    for(Student*p=stu;p<stu+5;p++){
        cout<<"请输入id为"<<(p-stu)+1<<"的学生的姓名"<<endl;
        cin>>p->name;
        cout<<"请输入"<<p->name<<"同学的成绩"<<endl;
        cin>>p->score;
    }
    float sum;
    for(Student*p=stu;p<stu+5;p++){
        sum += p->score;
    }
    float aver=sum/5;
    cout<<"平均分为"<<aver;
    return 0;
}