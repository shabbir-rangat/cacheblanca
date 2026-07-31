#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;
int main() {
  int listening = socket(AF_INET, SOCK_STREAM, 0);
  if (listening == -1) {
    cerr << "cant create a socket";
    return -1;
  }

  int opt = 1;
  if (setsockopt(listening, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) ==
      -1) {
    perror("setsockopt failed");
    return -1;
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
  fd_set master, dummy;
  FD_ZERO(&master);
  FD_SET(listening, &master);
  vector<int> clients;  
  while (true) {
    int maxfds = listening;
    int socketSize = select(maxfds + 1, &dummy, nullptr, nullptr, nullptr);
    if (socketSize == -1) {
      perror("select failed");
      break;
    }
    // FD_SETSIZE is 1024 default in linux
    for (int i = 0; i < maxfds; ++i) {
      	if(FD_ISSET(i,&dummy)){
		if(i==listening){
			sockaddr_in clientfd;
			socklen_t clientSize = sizeof(clientfd);
			int acceptedfd = accept(listening,( sockaddr*) clientfd , &clientSize );
			 if (clientFd == -1) {
         			 perror("accept failed");
         			 continue;
     			   }
			FD_SET(acceptedfd , &master);
			
        	if (clientFd > maxfds) {
         		 maxfds = clientFd;              // keep maxfds accurate
       		 }
       		 clients.push_back(clientFd);

       		cout << "New client connected: fd=" << clientFd << "\n";
		
		}
	}
    }
  }

  close(clientSocket);

  return 0;
}
