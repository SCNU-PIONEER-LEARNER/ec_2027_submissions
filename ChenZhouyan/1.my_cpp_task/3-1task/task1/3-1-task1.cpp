#include <iostream>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#endif

#include "prime_utils.h"

using namespace std;

int main() {
#ifdef _WIN32
    // 让 Windows 终端按 UTF-8 显示中文，避免乱码
    SetConsoleOutputCP(CP_UTF8);
#endif

    int n = 0;
    cout << "请输入一个整数 n: ";
    cin >> n;

    auto primes = primesUpTo(n);
    cout << "1 到 " << n << " 之间的质数有:" << endl;
    for (int p : primes) {
        cout << p << " ";
    }
    cout << endl;
    cout << "共有 " << countPrimes(n) << " 个质数" << endl;

#ifdef _WIN32
    // 停在结果页，按任意键再关窗口（否则弹出的窗口会一闪而过）
    system("pause");
#endif
    return 0;
}
