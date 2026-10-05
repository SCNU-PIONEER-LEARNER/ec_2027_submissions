#include <iostream>
using namespace std;

float rangemap(float m, float n, float x) {
    return 2.0 * (x - m) / (n - m) - 1.0;
}

int main() {
    float y = rangemap(1, 5, 4);
    cout << "y = " << y << endl;

    cout << "rangemap(1, 5, 1) = " << rangemap(1, 5, 1) << endl;
    cout << "rangemap(1, 5, 5) = " << rangemap(1, 5, 5) << endl;

    return 0;
}
