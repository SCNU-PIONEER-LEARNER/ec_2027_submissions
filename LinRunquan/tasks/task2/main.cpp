#include <iostream>
#include <cstring>
using namespace std;
int main()
{
    struct student{char name[20];int id;float score;};
    student stu[5];
    student *p;
    p=stu;
    float sum=0;
    for(int i=0;i<5;i++)
    {
cin>>p->name>>p->score;
        p->id=i+1;
        sum=sum+p->score;
        p++;
    }
    float avg;
avg=sum/5;
    cout<<avg<<endl;
    return 0;
}
