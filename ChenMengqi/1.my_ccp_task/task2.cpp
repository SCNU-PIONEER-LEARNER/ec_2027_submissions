#include<iostream>
#include<string>
using namespace std;
int main()
{
float score,average,sum;
sum=0;
struct student{
string name;
int id;
float score;
};
struct student stu[5];
student* p= stu;
int id=0;
for(p=stu;p<stu+5;p++)
{
id++;
p->id=id;
cout<<"Please enter the name and score"<<endl;
cin>>p->name>>p->score;
sum+=p->score;
}
average=sum/5.0;
cout<<average;
}