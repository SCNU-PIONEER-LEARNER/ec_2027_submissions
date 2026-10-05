#include <iostream>
#include <string>
using namespace std;

struct Student {
    string name;
    int    id;
    float  score;
};

int main() {
    Student stu[5];
    Student *p = stu;

    float sum = 0;

    for (int i = 0; i < 5; i++) {
        p[i].id = i + 1;

        cout << "输入第" << i + 1 << "个学生 姓名 成绩: ";
        cin >> p[i].name >> p[i].score;

        sum = sum + p[i].score;
    }

    float average = sum / 5.0;

    cout << endl << "平均成绩: " << average << endl;

    cout << "--- 学生名单 ---" << endl;
    for (int i = 0; i < 5; i++) {
        cout << "id=" << p[i].id
             << "  姓名=" << p[i].name
             << "  成绩=" << p[i].score << endl;
    }

    return 0;
}
