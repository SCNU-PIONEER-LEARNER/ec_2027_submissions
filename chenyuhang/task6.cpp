#include <iostream>
using namespace std;
class Geometry{public:
    
    virtual double vol()=0;
    virtual double biaos()=0;

};
class Square : public Geometry {
    private: double d ;
    public:
    Square (double d1){d=d1;}
    double vol(){return d*d*d ;}
    double biaos() {return 6*d*d ;}
};
class Spherome : public Geometry {
    private: double r;
    public:
    Spherome (double r1){r=r1;}
    double vol(){return 4*3.1415926*r*r*r/3 ;}
    double biaos(){return 4*3.1415926*r*r ;}
};
int main(){
    Square zhi(6);
    Spherome zhi1(8);
    cout<<"正方体的体积和表面积分别为"<<zhi.vol()<<endl<<zhi.biaos()<<endl;
    cout<<"球体的体积和表面积分别为"<<zhi1.vol()<<endl<<zhi1.biaos()<<endl;

    return 0;
}