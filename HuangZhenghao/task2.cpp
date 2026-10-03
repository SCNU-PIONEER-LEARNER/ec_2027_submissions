#include <iostream>
using namespace std;

    struct Student{
        string name;
        int id;
        float score;

    };
int main() {
    Student stu[5];
    Student*p=stu;
    float sum=0.0f;
    for(int i=0;i<5;i++,p++)
    {
        p->id=i+1;
        cout<<"第"<<p->id<<"位学生名字:";
        cin>>p->name;
        cout<<"成绩:";
        cin>>p->score;
        sum+=p->score;
    }
    float avg=sum/5.0f;  

    std::cout <<"五位学生的平均成绩为："<<avg<< std::endl;
    return 0;
}
