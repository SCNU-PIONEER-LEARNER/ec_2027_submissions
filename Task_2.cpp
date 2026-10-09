#include <iostream>
#include <string>
using namespace std;
struct Student { string name; 
                int id; 
                float score; 
            };
int main() {
    Student stu[5]; 
    Student *p = stu; 
    float average,sum = 0;
    for (int i = 0; i < 5; i++) {
        cout << "Input name and score for the student" << i+1 << ": ";
        cin >> (p+i)->name >> (p+i)->score; 
        (p+i)->id = i + 1; 
        sum += (p+i)->score;
    }
    average=sum/5.0;
    cout << "Average score: " <<average<< endl;
    return 0;
}