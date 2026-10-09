#include <iostream>
using namespace std;
float rangemap(float m, float n, float x) {
    return (x - m) / (n - m) * 2.0 - 1.0; 
}
int main() { 
    cout << "Outcome: " << rangemap(1, 5, 4) << endl; 
    return 0; 
}
