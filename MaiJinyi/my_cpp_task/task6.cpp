#include <iostream>
using namespace std;
const float PI=3.14159f;
class Geometry{
    public:
        virtual float calcVolume()=0;
        virtual float calcArea()=0;
};
class Square:public Geometry{
    private:
        float side;
    public:
        Square(float s){
            side=s;
        }
        float calcVolume(){
            return side*side*side;
        }
        float calcArea(){
            return 6*side*side;
        }
};
class Spherome:public Geometry{
    private:
        float radius;
    public:
        Spherome(float r){
            radius=r;
        }
        float calcVolume(){
            return (4.0f/3.0f)*PI*radius*radius*radius;
        }
        float calcArea(){
            return 4*PI*radius*radius;
        }
};
int main(){
    Square a(3.0f);
    cout<<"square"<<endl;
    cout<<"volume:"<<a.calcVolume()<<endl;
    cout<<"area:"<<a.calcArea()<<endl;

    Spherome b(2.0f);
    cout<<"spherome"<<endl;
    cout<<"volume:"<<b.calcVolume()<<endl;
    cout<<"area:"<<b.calcArea()<<endl;
    return 0;
}