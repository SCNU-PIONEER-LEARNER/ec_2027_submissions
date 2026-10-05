#include <iostream>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#endif

// 将 (m, n) 区间线性归一化到 (-1, 1) 区间
// 推导：先把 (m,n) 映射到 (0,1)：t = (x-m)/(n-m)
//        再映射到 (-1,1)：y = 2*t - 1 = (2x - m - n)/(n - m)
float rangemap(float m, float n, float x) {
    return (2.0f * x - m - n) / (n - m);
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);  // 让 Windows 终端正确显示中文
#endif
    float m, n, x;
    std::cout << "请输入 m, n, x（要求 m < x < n）: ";
    std::cin >> m >> n >> x;

    float y = rangemap(m, n, x);
    std::cout << "归一化后的 y = " << y << std::endl;

    system("pause");  // 按任意键才关闭窗口，避免一闪而过
    return 0;
}
