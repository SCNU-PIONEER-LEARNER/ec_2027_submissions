#include <iostream>
#ifdef _WIN32
#include <windows.h>
static double PI =3.1415926535;
#endif
using namespace std;
class Geometry{
        public:
            virtual float calculate_V()=0;
            virtual float calculate_S()=0;
            float V;
            float S;
    };
class Square : public Geometry {
    public:
            
    float calculate_V() override{
            V=length*length*length;
            return V;
        };
    float calculate_S() override{
            S=6*length*length;
            return S;
        };
    Square(float length)
    {
        this->length=length;
    };
    private:
    float length;
    };
class Spherome : public Geometry {
    public:
    float calculate_V() override{
        V=(4/3)*PI*r*r*r;
        return V;
    };
    float calculate_S() override{
        S=4*PI*r*r;
        return S;
    };
    Spherome(float r){
        this->r=r;
    };
    private:
    float r;
    };
int main() {
#ifdef _WIN32
    // 设置控制台按 UTF-8 解读输出,解决中文乱码(仅 Windows 生效,不影响 Linux)
    SetConsoleOutputCP(CP_UTF8);
#endif
    float length,r;
    cout<<"请输入正方体的边长"<<endl;
    cin>>length;
    cout<<"请输入球的半径"<<endl;
    cin>>r;
    Square p(length);
    float Va= p.calculate_V();
    float Sa= p.calculate_S();
    Spherome q(r);
    float Vb = q.calculate_V();
    float Sb = q.calculate_S();
    cout <<"正方体的体积是："<<Va<<";表面积是："<<Sa<<endl;
    cout <<"球的体积是："<<Vb<<";表面积是："<<Sb<<endl;
    return 0;
}