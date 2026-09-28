#include <iostream>
using namespace std;

int main() {
    int count = 0;
    int n;
    cin >> n;

    for (int i = 2; i <= n; i++) {
        int j;
        for (j = 2; j < i; j++) {
            if (i % j == 0) {
                break;
            }
        }
        if (j == i) {
            cout << i << " ";  
            count++;
        }
    }

    cout << endl << count << endl;
    return 0;
}