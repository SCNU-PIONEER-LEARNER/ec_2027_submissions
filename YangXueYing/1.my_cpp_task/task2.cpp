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

    Student *p = stu;

    for(int i = 0; i < 5; i++,p++){
        cout <<"please enter the " << i+1 << " student_name and_grade";
        cin >> (*p).name >>(*p).score;
        (*p).id = i + 1;

    }

    p = stu;
    float sum = 0;
    for(int i = 0;i < 5;i++, p++){
        sum += (*p).score;
    }

    cout << sum / 5 <<endl;

    return 0;
}