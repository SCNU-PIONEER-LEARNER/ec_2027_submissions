#include <iostream>
using namespace std;
int main(){
	int n;
	cin>>n;
	
	for(int n1=2;n1<n;n1++)
	{
		bool ok=true;
		for(int n2=2;n2<n1;n2++)
		{
			if(n1%n2==0){
				ok=false;
				
				break;
			}
			
		}
		if (ok==true){
			cout<<n1;
		}
		
	 } 
	 return 0;
} 
