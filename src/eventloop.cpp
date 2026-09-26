#include "eventloop.hpp"
#include "client.hpp"
#include "tcp_server.hpp"
#include <cstdint>
#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>
using namespace std;

eventloop::eventloop() {
  epfd = epoll_create1(0);
  if (epfd == -1) {
    cerr << "epoll_create1 failed" << endl;
  }
}
eventloop::~eventloop() {
  if (epfd != -1) {

    close(epfd);
  }
}
void eventloop::addFd(int fd, uint32_t event) {
  struct epoll_event ev{};
  ev.events = event;
  ev.data.fd = fd;
  if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
    cerr << "epoll_ctl add failed" << endl;
  }
}
void eventloop::removefd(int fd) {
  if (fd < 0)
    return;

  // Issue the system call to remove fd from epoll's interest list
  if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr) == -1) {
    // Log or handle errors (e.g., ENOENT if fd wasn't registered)

    std::cerr << "error in remove fd function" << endl;
  }
}
void eventloop::run(int serversocket) {
  addFd(serversocket, EPOLLIN);
  struct epoll_event ready[max_event];
  while (true) {
    int n = epoll_wait(epfd, ready, max_event, -1);
    if (n == -1) {
      cerr << "epoll wait failed" << endl;
      continue;
    }
    for (int i = 0; i < n; ++i) {
      // handle t:wq
      // he fds
      int clientfd = ready[i].data.fd;
      if (ready[i].data.fd == serversocket) {
        handle_new_connection(serversocket, *this);

      } else {

        handle_client_read(clientfd, *this);
      }
    }
  }
}
