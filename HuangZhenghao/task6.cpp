#include <iostream>
const double PI=3.14159265;
class Geometry{
    public:
    virtual double getVolume() const=0;
    virtual double getSurfaceArea() const=0;
};
class Cube:public Geometry{
    private:
    double s;
    public:
    Cube(double input_s){
        s=input_s;
    }
    double getVolume() const{
        return s*s*s;}
    double getSurfaceArea() const{
        return 6*s*s;
    }    
};



class Sphere:public Geometry{
    private:
    double r;
    public:
    Sphere(double input_r){
        r=input_r;
    }
    double getVolume() const{
        return (4.0/3)*PI*r*r*r;}
    double getSurfaceArea() const{
            return 4*PI*r*r;
        } 
    };


int main() {
    double s,r;
    std::cout <<"请输入正方体边长:" << std::endl;
    std::cin>>s;
    Cube myCube(s);
    std::cout<<"正方体体积:"<<myCube.getVolume()<<std::endl;
    std::cout<<"正方体表面积:"<<myCube.getSurfaceArea()<<std::endl;
    std::cout <<"请输入球体半径:" << std::endl;
    std::cin>>r;
    Sphere myBall(r);
    std::cout<<"球体体积:"<<myBall.getVolume()<<std::endl;
    std::cout<<"球体表面积:"<<myBall.getSurfaceArea()<<std::endl;
    return 0;
}
