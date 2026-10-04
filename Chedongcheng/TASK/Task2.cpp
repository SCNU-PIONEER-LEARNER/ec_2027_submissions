#include <iostream>
#include <string>
using namespace std;

int main() {
    system("chcp 65001");
    struct student {
        string name;
        int id=1;
        float score;
    };
    student a[5];
    student *p = a;

    cout << "现在录入数据" << endl;
    for (int i = 1; i < 6; i++) {
        cout << "请输入第" << i << "名同学的名字：";
        cin >> p[i-1].name;
        cout << "请输入第" << i << "名同学的成绩：";
        cin >> p[i-1].score;
    }
    for (int i=0;i<5;i++){
        p[i].id=i+1;
        cout<<p[i].id<<"\t"<<p[i].name<<"\t"<<p[i].score<<endl;
       
    }
    float b=0;
    for(int i=0;i<5;i++){b+=p[i].score;}
    float c=b/5;
    cout<<"平均分为："<<c<<endl;
    return 0;
}