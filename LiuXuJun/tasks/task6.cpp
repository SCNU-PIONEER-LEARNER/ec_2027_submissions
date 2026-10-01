#include"iostream"
#include"cmath"
using namespace std;
const float PI=acos(-1);
class jiheti{
    public:
    virtual float getV()=0;
    virtual float getS()=0;
};
class zhengfangti:public jiheti{
    private:
        float a;
    public:
        zhengfangti(float cina){
            a=cina;
        }
    float getV()override{
        return a*a*a;
    
    }
    float getS()override{
        return a*a*6;
    
    }
};
class qiu:public jiheti{
    private:
        float r;
    public:
        qiu(float cinr){
            r=cinr;
        }
    float getV()override{
        return (4.0/3.0)*PI*r*r*r;
    
    }
    float getS()override{
        return 4*PI*r*r;
    }
};
int main(){
    float cina,cinr;
    cout<<"请输入边长："<<endl;
    cin>>cina;
    cout<<"请输入半径："<<endl;
    cin>>cinr;
    zhengfangti zhengfangtiA(cina);
    qiu qiuA(cinr);
    cout<<"正方体体积和表面积："<<zhengfangtiA.getV()<<endl<<zhengfangtiA.getS();
    cout<<"球体积和表面积："<<qiuA.getV()<<endl<<qiuA.getS();

    return 0;
}
