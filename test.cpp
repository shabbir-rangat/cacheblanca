#include<cstring>
#include<string.h>
#include<iostream>
using namespace std;
int main(){
	int arr[5];
	int size = sizeof(arr) / sizeof(arr[0]);
	memset(arr,0,sizeof(arr));
	for(int i = 0; i < size; ++i){
		cout<<arr[i]<<endl;
	}
	return 0;
}
