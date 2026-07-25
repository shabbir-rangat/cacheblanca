#include<iostream>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<string.h>
using namespace std;
int main(){
	//create a socket
	//bind the socket to the local port
	//
	int listening = socket(AF_INET,SOCK_STREAM,0);
	if(listening == -1 ){
		cerr<<"cant create a socket";
		return -1;
	}else{
		cout<<"listening to the port "<<listening<<endl;
	}

}	
