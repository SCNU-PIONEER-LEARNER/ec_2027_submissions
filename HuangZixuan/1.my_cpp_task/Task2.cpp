#include<iostream>

using namespace std;

struct Student{
    string name;
    int age;
    float score;
};

int main(){
    Student Stu[5];
    for(int i{0};i<5;++i){
        cin>>Stu[i].name>>Stu[i].age>>Stu[i].score;
    }
    float average{0};
    Student *p = Stu;
    for(Student *p = Stu; p < Stu + 5; ++p){
        average += p->score;
    }
    average /= 5;
    cout<<"Average score: "<<average<<endl;
}