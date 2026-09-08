#include <iostream>
using namespace std;

class Geometry {
	public:
		virtual double getvolume()=0;
		virtual double getarea()=0;
};

class Square : public Geometry{
	private :
		double side;
	public:
		Square(double s){
			side=s;
		}
	double getvolume(){
		return side*side*side;
	}
	double getarea(){
		return side*side*6;
	}
};

class Spherome:public Geometry{
	private:
		double side;
	public:
		Spherome(double r){
			side=r;
		}
		double getvolume(){
			return 4/3*3.1415926*side*side*side;
		}
		double getarea(){
			return 4*3.1415926*side*side;
		}
};

int main(){
	cout<<"请输入正方体边长"; 
	double a;
	cin>>a;
	Square cube(a);
	cout<<"请输入球的半径";
	double b;
	cin>>b;
	Spherome ball(b);
	
	cout<<"正方体体积大小"<<cube.getvolume()<<endl;
	cout<<"正方体面积大小"<<cube.getarea()<<endl;
	cout<<"球体积大小"<<ball.getvolume()<<endl;
	cout<<"球面积大小"<<ball.getarea()<<endl;
	
	return 0; 
	 
	
}
