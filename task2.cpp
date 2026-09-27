#include <iostream>
#include <string>
using namespace std;
struct Student
{
    string name;
    int id;
    float score;
};
int main()
{
    Student stu[5];
    Student *p=stu;
    float average;
    float sum=0;
for(int i=0;i<5;i++)
{
    p->id=i+1;
    cout<<"请输入第"<<p->id<<"个学生姓名、成绩：";
    cin>>p->name>>p->score;
    sum=sum+p->score;
    p++;
}
average =sum/5.0;
p=stu;
for(int i=0;i<5;i++)
{
cout<<"id:"<<p->id<<"姓名:"<<p->name<<"成绩:"<<p->score<<endl;
p++;
}
cout<<"5名学生平均成绩="<<average<<endl;
return 0;
}