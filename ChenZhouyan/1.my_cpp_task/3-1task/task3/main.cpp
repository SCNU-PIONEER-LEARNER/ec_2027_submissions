#include <iostream>
#include <windows.h>

using namespace std;

// 把 (m, n) 区间里的数 x，换算到 (-1, 1) 区间里
// 思路：x 离 m 越近结果越接近 -1，离 n 越近结果越接近 1
float rangemap(float m, float n, float x) {
    // 先算 x 在 (m,n) 里跑了百分之多少，再乘 2 减 1 挪到 (-1,1)
    float t = (x - m) / (n - m);   // 百分比，在 0 到 1 之间
    return 2 * t - 1;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);   // 让终端正常显示中文

    float m, n, x;
    cout << "请输入 m, n, x（要求 m < x < n）: ";
    cin >> m >> n >> x;

    float y = rangemap(m, n, x);
    cout << "归一化后的 y = " << y << endl;

    system("pause");   // 按任意键再关窗口
    return 0;
}
