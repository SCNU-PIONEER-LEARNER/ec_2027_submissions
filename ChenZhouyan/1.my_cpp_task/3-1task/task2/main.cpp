#include <iostream>
#include <string>
#include <windows.h>

using namespace std;

// 结构体：姓名 + 学号 + 成绩
struct Student {
    string name;
    int id;
    float score;
};

int main() {
    // 让终端正常显示中文
    SetConsoleOutputCP(CP_UTF8);

    Student stu[5];      // 长度为 5 的学生数组
    Student *p = stu;    // 指针 p 指向数组第一个学生

    float sum = 0;       // 存总成绩

    // 输入 5 个学生的信息
    for (int i = 0; i < 5; i++) {
        p->id = i + 1;   // 学号从 1 到 5

        cout << "请输入第 " << i + 1 << " 位学生的姓名: ";
        cin >> p->name;

        cout << "请输入第 " << i + 1 << " 位学生的成绩: ";
        cin >> p->score;

        sum = sum + p->score;
        p = p + 1;       // 指针往后移一位，指向下一个学生
    }

    float average = sum / 5;   // 平均成绩

    // 指针拨回数组开头，再输出一遍所有信息
    p = stu;
    cout << "\n----- 5 名学生信息 -----" << endl;
    for (int i = 0; i < 5; i++) {
        cout << "id=" << p->id
             << " 姓名=" << p->name
             << " 成绩=" << p->score << endl;
        p = p + 1;
    }

    cout << "平均成绩: " << average << endl;

    system("pause");   // 按任意键再关窗口，不然一闪就没了
    return 0;
}
