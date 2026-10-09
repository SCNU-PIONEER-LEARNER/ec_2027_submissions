#include <iostream>
#include <cmath>
using namespace std;

int main() {
    float x, y, alpha, theta, a, b;
    cout << "Enter x, y, alpha (in degrees), theta (in degrees): ";
    cin >> x >> y >> alpha >> theta;
    float theta_rad = theta * 3.14159 / 180.0
    a = x * cos(theta_rad) - y * sin(theta_rad);
    b = x * sin(theta_rad) + y * cos(theta_rad);
    cout << "New coordinates: a = " << a << ", b = " << b << endl;
    return 0;
}