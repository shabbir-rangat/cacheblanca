#include<iostream>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<string.h>
using namespace std;
int main(){
	int listening = socket(AF_INET,SOCK_STREAM,0);
	if(listening == -1 ){
		cerr<<"cant create a socket";
		return -1;
	}
	

int opt = 1;
if(setsockopt(listening, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1){
    perror("setsockopt failed");
    return -1;
}	
	// binding the socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.s_addr = INADDR_ANY;
	inet_pton(AF_INET,"0.0.0.0" , &hint.sin_addr);
	if(bind(listening,(sockaddr*)&hint, sizeof(hint))==-1){
		cerr<<"cant bind to ip/port";
		perror("bind failed");
		return -2;
	}
	if(listen(listening, SOMAXCONN)==-1){
		cerr<<"cant listen!";
		return -3;
	}
	sockaddr_in client;
	socklen_t clientsize = sizeof(client);
	char host[NI_MAXHOST];
	char service[NI_MAXSERV];

	//accept()
	int clientSocket = accept(listening, (sockaddr*)&client,&clientsize);
	if(clientSocket==-1){
		cerr<<"problem client connection!";
		return -4;
	}
	close(listening);
	memset(host,0,NI_MAXHOST);
	memset(service, 0 , NI_MAXSERV);
	int result = getnameinfo((sockaddr*)&client , clientsize,host,NI_MAXHOST,service,NI_MAXSERV,0 );

	if(result==0){
		cout<<host<<"connected on "<<service<<endl;
	}
	else{
		inet_ntop(AF_INET,&client.sin_addr,host,NI_MAXHOST);
		cout<<host<<"connected on "<<ntohs(client.sin_port)<<endl;

	}
	char buff[4096];
	while(true){
		memset(buff,0,4096);
		int byteRecv = recv(clientSocket, buff, 4096,0);
		if(byteRecv==-1){
			cerr<<"there was a connection issue";
			break;
		}
		if(byteRecv==0){
			cout<<"client disconected";
			break;
		}
		cout << "Recieved: "<< string(buff,0,byteRecv)<<endl;
		send(clientSocket,buff,byteRecv+1,0);
	
	}																									
		close(clientSocket);


	return 0;
}
