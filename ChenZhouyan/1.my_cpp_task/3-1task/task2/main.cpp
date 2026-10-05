#include <iostream>
#include <string>
#include <windows.h>

using namespace std;

// 结构体：把姓名、学号、成绩打包在一起
struct Student {
    string name;
    int id;
    float score;
};

int main() {
    SetConsoleOutputCP(CP_UTF8);   // 让终端正常显示中文

    Student stu[5];    // 5 个学生
    float sum = 0;     // 总成绩

    // 输入 5 个学生的信息
    for (int i = 0; i < 5; i++) {
        stu[i].id = i + 1;   // 学号从 1 排到 5

        cout << "请输入第 " << i + 1 << " 位学生的姓名: ";
        cin >> stu[i].name;

        cout << "请输入第 " << i + 1 << " 位学生的成绩: ";
        cin >> stu[i].score;

        sum = sum + stu[i].score;   // 累加成绩
    }

    // 再循环一遍，把所有信息打印出来
    cout << "\n----- 5 名学生信息 -----" << endl;
    for (int i = 0; i < 5; i++) {
        cout << "学号=" << stu[i].id
             << " 姓名=" << stu[i].name
             << " 成绩=" << stu[i].score << endl;
    }

    cout << "平均成绩: " << sum / 5 << endl;

    system("pause");   // 按任意键再关窗口，不然一闪就没了
    return 0;
}
