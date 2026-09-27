#include <iostream>
#include <cmath>
using namespace std;

float radian(float degree){
    return degree / 180.0 * 3.1415926;
}

float convertx(float x,float y, float theta){
    return x * cos(radian(theta)) - y * sin(radian(theta));
}

float converty(float x, float y,float theta){
    return x * sin(radian(theta)) + y * cos(radian(theta));

}

int main(){
    float x, y, alpha, theta;
    cout << "please enter x, y, alpha, theta:";
    cin >> x >> y >> alpha >> theta;

    float a = convertx(x, y, theta);
    float b = converty(x, y, theta);

    cout << "(" << a << "," << b << ")" << endl;




    return 0;
}
