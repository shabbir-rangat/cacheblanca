#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include<string>
using namespace std;

int main() {
  int listening = socket(AF_INET, SOCK_STREAM, 0);
  if (listening == -1) {
    cerr << "cant create a socket";
    return -1;
  }

  int opt = 1;
  if (setsockopt(listening, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
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
  int maxfds = listening;

  vector<int> clients;  // track connected client fds ourselves

  while (true) {
    dummy = master;  // select() mutates it, so always work off a fresh copy

    int socketCount = select(maxfds + 1, &dummy, nullptr, nullptr, nullptr);
    if (socketCount == -1) {
      perror("select failed");
      break;
    }

    // loop over every possible fd up to maxfds, not up to socketCount
    for (int i = 0; i <= maxfds; ++i) {
      if (!FD_ISSET(i, &dummy)) continue;

      if (i == listening) {
        // new connection pending
        sockaddr_in clientAddr;
        socklen_t clientSize = sizeof(clientAddr);
        int clientFd = accept(listening, (sockaddr *)&clientAddr, &clientSize);

        if (clientFd == -1) {
          perror("accept failed");
          continue;
        }

        FD_SET(clientFd, &master);        // start watching this new client
        if (clientFd > maxfds) {
          maxfds = clientFd;              // keep maxfds accurate
        }
        clients.push_back(clientFd);

        cout << "New client connected: fd=" << clientFd << "\n";

      } else {
        // existing client has data (or disconnected)
        char buf[4096];
        memset(buf, 0, sizeof(buf));
        int bytesRead = recv(i, buf, sizeof(buf), 0);
	if(bytesRead > 0){
		std::string message(buf, bytesRead);

	}
        if (bytesRead <= 0) {
          // 0 = clean disconnect, <0 = error
          if (bytesRead == 0) {
            cout << "Client disconnected: fd=" << i << "\n";
          } else {
            perror("recv failed");
          }
          close(i);
          FD_CLR(i, &master);

          // remove from our tracking vector
          for (size_t k = 0; k < clients.size(); ++k) {
            if (clients[k] == i) {
              clients.erase(clients.begin() + k);
              break;
            }
          }

        } else {
	cout<<"client fd = "<<i<<" messaged: "<<buf<<endl;
	 std::string message(buf, bytesRead);
	 std:: string response;
	 if(message=="ping\r\n" || message=="PING\r\n"){
	 	response = "pong!";
	 }else{
	 	
          send(i, buf, bytesRead, 0);
	 }
	 send(i,response.c_str(), response.size(), 0 );
	 


          // got data — echo it back for now
        }
      }
    }
  }

  close(listening);
  for (int fd : clients) {
    close(fd);
  }
  return 0;
}
