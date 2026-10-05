#include <iostream>
using namespace std;

int main() {
    int n = 0;
    cout << "输入 n: ";
    cin >> n;

    int count = 0;

    for (int i = 2; i <= n; i++) {

        int m;
        for (m = 2; m < i; m++) {
            if (i % m == 0) {
                break;
            }
        }

        if (m == i) {
            cout << i << " ";
            count++;
        }
    }

    cout << endl << "共 " << count << " 个质数" << endl;
    return 0;
}
