#pragma once

#include <cstdint>
#include <sys/epoll.h>

class eventloop {
private:
  int epfd;
  const int max_event = 1024; // random-number

public:
  eventloop();
  ~eventloop();
  void addFd(int fd, uint32_t events);
  void run(int serversocket);
  void removefd(int fd);
};
