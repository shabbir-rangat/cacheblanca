#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <string>


#include <fcntl.h>
using namespace std;
  int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return -1;

    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main() {
  int listening = socket(AF_INET, SOCK_STREAM, 0);
  if (listening == -1) {
    cerr << "cant create a socket";
    return -1;
  }

  int opt = 1;
  if (setsockopt(listening, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) ==
      -1) {

  }
  // binding the socket
  sockaddr_in hint;
  hint.sin_family = AF_INET;
  hint.sin_port = htons(54000);
  hint.sin_addr.s_addr = INADDR_ANY;
  inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);
  if (bind(listening, (sockaddr *)&hint, sizeof(hint)) == -1) {
    cerr << "cant bind to ip/port";
    perror("bind failed");
    return -2;
  }
  if (listen(listening, SOMAXCONN) == -1) {
    cerr << "cant listen!";
    return -3;
  }
// the socket is opened for listning 
// accept multple clients using epoll

  int epfd = epoll_create1(0);
  struct epoll_event ev{};
  //ev has events and data
  ev.events = EPOLLIN;
  ev.data.fd = listening;
  epoll_ctl(epfd,EPOLL_CTL_ADD,listening,&ev );
 while(true){
  	//watch for new clients
	const int MAX_EVENT = 100; // you can put any int 
	struct epoll_event ready[MAX_EVENT];
	int n	= epoll_wait(epfd, ready, MAX_EVENT,-1);// will return the fds that are ready z
	for(int i  = 0; i < n ; ++i ){
		if(ready[i].data.fd==listening){
			//if a new client connect()
			sockaddr_in sfd;
			socklen_t sfd_size = sizeof(sfd);

			int afd = accept(listening,(sockaddr*)&sfd, &sfd_size );
			if(set_nonblocking(afd)==-1){
				cerr<<"fcntl"<<endl;
			}else{ 
				set_nonblocking(afd);
				struct epoll_event cev{};
           			 cev.events = EPOLLIN;
				 cev.data.fd = afd;
           			 epoll_ctl(epfd, EPOLL_CTL_ADD,afd, &cev);
			
			}

		}else{
			//an existing client tries to send() or recv();
			int clientfd = ready[i].data.fd;
			char buff[4096];
			ssize_t count = recv(clientfd,buff,sizeof(buff),0);
			if(count<=0){
				epoll_ctl(epfd,EPOLL_CTL_DEL,clientfd,nullptr);
				close(clientfd);
			}else{
				cout<<"client connected = " << clientfd<< endl;
				string str(buff,count);
				cout<<"client sent "<<str<<endl; 
				send(clientfd,buff,count,0);


			}
			
		}
	}
  }

  close(listening);

  return 0;
}
