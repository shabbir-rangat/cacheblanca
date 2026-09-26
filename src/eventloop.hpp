#pragma once

#include "Store.hpp"
#include "client.hpp"
#include <cstdint>
#include <memory>
#include <sys/epoll.h>
#include <unordered_map>
class eventloop {
private:
  int epfd;
  const int max_event = 1024; // random-number

public:
  eventloop();
  ~eventloop();
  std::unordered_map<int, std::unique_ptr<Client>> clients;
  store myStore;
  void addFd(int fd, uint32_t events);
  void run(int serversocket);
  void removefd(int fd);
};
