#include "tcp_server.hpp"
#include "Store.hpp"
#include "cbdecoder.hpp"
#include "client.hpp"
#include "dispatcher.hpp"
#include "eventloop.hpp"
#include <arpa/inet.h>
#include <cstddef>
#include <fcntl.h>
#include <iostream>
#include <memory>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
int set_nonblocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1)
    return -1;

  return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int init_server_socket(int port) {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    std::cerr << "Failed to create server socket" << std::endl;
    return -1;
  }

  // 2. Allow port reuse immediately on restart (prevents "Address already in
  // use")
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) <
      0) {
    std::cerr << "setsockopt SO_REUSEADDR failed" << std::endl;
    close(server_fd);
    return -1;
  }

  // 3. Bind socket to the specified port
  sockaddr_in address{};
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY; // Listen on all network interfaces
  address.sin_port = htons(port);       // Convert to network byte order

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    std::cerr << "Bind failed on port " << port << std::endl;
    close(server_fd);
    return -1;
  }

  // 4. Start listening for incoming connections
  if (listen(server_fd, SOMAXCONN) < 0) {
    std::cerr << "Listen failed" << std::endl;
    close(server_fd);
    return -1;
  }

  // 5. Force the listening socket to be non-blocking for epoll
  if (set_nonblocking(server_fd) == -1) {
    std::cerr << "Failed to set server socket non-blocking" << std::endl;
    close(server_fd);
    return -1;
  }

  return server_fd;
}
void handle_new_connection(int serversocket, eventloop &loop) {
  sockaddr_in sfd;
  socklen_t sfd_size = sizeof(sfd);

  int afd = accept(serversocket, (sockaddr *)&sfd, &sfd_size);

  if (afd == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK)
      return; // False alarm, loop back
    std::cerr << "Accept error" << std::endl;
    return;
  }

  if (set_nonblocking(afd) == -1) {
    std::cerr << "fcntl" << std::endl;
    close(afd);
    return;
  }
  auto client = std::make_unique<Client>();
  client->fd = afd;
  loop.clients.emplace(afd, std::move(client));

  loop.addFd(afd, EPOLLIN);
}
void handle_client_read(int clientfd, eventloop &loop) {
  auto it = loop.clients.find(clientfd);
  if (it == loop.clients.end()) {
    std::cerr << "unknown client fd" << std::endl;
    return;
  }
  Client &client = *(it->second);
  char buff[1024];
  ssize_t num_of_bytes_recv = recv(clientfd, buff, sizeof(buff), 0);
  std::cerr << "DEBUG: recv() returned " << num_of_bytes_recv << " bytes on fd"
            << clientfd << "\n";

  if (num_of_bytes_recv <= 0) {
    loop.removefd(clientfd); // needs loop's epfd_ internally
    return;
  }
  client.read_buffer.append(buff, num_of_bytes_recv);
  while (true) {
    size_t consumed = 0;
    command cmd;
    auto result = decode_command(client.read_buffer, consumed, cmd);
    if (result == parserResult::INCOMPLETE) {
      break;
    }
    if (result == parserResult::ERROR) {
      std::cerr << "protocol error on  fd" << clientfd << std::endl;
      loop.removefd(clientfd);

      return;
    }
    dispatche(cmd, client, loop.myStore);
    client.read_buffer.erase(0, consumed);
  }
  if (!client.write_buffer.empty()) {
    ssize_t sent = send(clientfd, client.write_buffer.data(),
                        client.write_buffer.size(), 0);

    if (sent > 0) {
      client.write_buffer.erase(0, sent);
    }
    // for v1: ignoring the case where `sent < write_buffer.size()` (partial
    // write) — fine for small replies, but a real EPOLLOUT-based flush would
    // handle it properly later
  }
}
