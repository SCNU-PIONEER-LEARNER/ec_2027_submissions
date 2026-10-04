 #include <iostream>
 using namespace std;
 #include <cmath>

 int main(){
   system("chcp 65001");
    const double pai= acos(-1.0);
    double a,b,x,y,alpha,theta;
    cout<<"请输入旋转前的坐标：(a,b)"<<endl;
    cin>>a>>b;
    cout <<"请输入旋转的角度:"<<endl;
    cin>>theta;
    double c=theta * pai/180;
    x=a*cos(c)-b*sin(c);
    y=a*sin(c)+b*cos(c);
     cout<<"旋转后的坐标为：（"<<x<<"，"<<y<<")"<<endl;
   return (0);
 }
 