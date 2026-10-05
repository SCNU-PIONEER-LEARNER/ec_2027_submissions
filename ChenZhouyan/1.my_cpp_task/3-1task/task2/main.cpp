#include <iostream>
#include <string>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#endif

// 结构体定义：姓名(字符串) + 学号(整型) + 成绩(浮点)
struct Student {
    std::string name;
    int id;
    float score;
};

#ifdef _WIN32
// 把控制台输入的中文（终端默认是 GBK 编码）转成 UTF-8，
// 否则中文姓名存进数组后，按 UTF-8 输出就会乱码
std::string consoleToUtf8(const std::string& input) {
    UINT cp = GetConsoleCP();               // 当前终端的输入编码
    if (cp == 0 || cp == CP_UTF8) return input;  // 无终端或已是 UTF-8，不转
    int wlen = MultiByteToWideChar(cp, 0, input.c_str(), -1, nullptr, 0);
    std::wstring wide(static_cast<size_t>(wlen), L'\0');
    MultiByteToWideChar(cp, 0, input.c_str(), -1, &wide[0], wlen);
    int ulen = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string utf8(static_cast<size_t>(ulen), '\0');
    WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, &utf8[0], ulen, nullptr, nullptr);
    utf8.resize(static_cast<size_t>(ulen) - 1);  // 去掉结尾的 \0
    return utf8;
}
#endif

int main() {
#ifdef _WIN32
    // 让 Windows 终端按 UTF-8 显示中文，避免乱码
    SetConsoleOutputCP(CP_UTF8);
#endif

    Student stu[5];        // 长度为 5 的 Student 类型数组
    Student *p = stu;      // 指针指向数组首元素

    float sum = 0.0f;      // 累加成绩，算平均用
    for (int i = 0; i < 5; i++) {
        p->id = i + 1;     // id 由 1 到 5 顺序排列
        std::cout << "请输入第 " << (i + 1) << " 位学生的姓名: ";
        std::cin >> p->name;
#ifdef _WIN32
        p->name = consoleToUtf8(p->name);   // 中文姓名编码转换
#endif
        std::cout << "请输入第 " << (i + 1) << " 位学生的成绩: ";
        std::cin >> p->score;
        sum += p->score;
        p++;               // 指针后移一位，指向下一位学生
    }

    float average = sum / 5.0f;   // 平均成绩

    // 指针回到数组开头，输出所有学生信息
    p = stu;
    std::cout << "\n--- 5 名学生信息 ---" << std::endl;
    for (int i = 0; i < 5; i++, p++) {
        std::cout << "id=" << p->id
                  << " 姓名=" << p->name
                  << " 成绩=" << p->score << std::endl;
    }
    std::cout << "平均成绩: " << average << std::endl;

#ifdef _WIN32
    // 停在结果页，按任意键再关窗口（否则弹出的窗口会一闪而过）
    system("pause");
#endif
    return 0;
}
