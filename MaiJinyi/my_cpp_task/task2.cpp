#include <iostream>
#include <string>
using namespace std;
struct Student{
    string name;
    int id;
    float score;
};
int main(){
    Student stu[5];
    Student *p=stu;
    float total_score=0;
    for(int i=0;i<5;i++){
        cout<<"please enter no."<<(i+1)<<" student's name and score:"<<endl;
        cin>>p->name>>p->score;
        p->id=i+1;
        total_score+=p->score;
        p++;
    }
float average=total_score/5.0;
cout<<"the average score of 5 students is:"<<average<<endl;
return 0;
}