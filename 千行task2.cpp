#include <iostream>
using namespace std;

struct student{
	    char name[20];
		int id;
		float store;
		
};
int main(){
	
	
	student stu[5];
	student*p=stu;
	for(int i=1;i<=5;i++){
		p->id=i;
		cin>>p->name;
		cin>>p->store;
		p++;
	}
	float avr;
	avr=(stu[0].store+stu[1].store+stu[2].store+stu[3].store+stu[4].store)/5;
	cout<<avr;
	
	return 0;
}
