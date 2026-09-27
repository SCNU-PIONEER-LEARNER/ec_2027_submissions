#include <iostream>
using namespace std;

float rangmap(float m,float n, float x){
    return (x - m) / (n - m) * 2.0 - 1.0;
}

int main(){
    float m,n,x;
    cout << "please enter m ,n ,x:";
    cin >> m >> n >> x;

    float y = rangmap(m, n, x);
    cout << "y=" << y << endl;

    return 0;
}