#include "eventloop.hpp"
#include "tcp_server.hpp"
#include <iostream>
using namespace std;

int main() {
  int serverSocket = init_server_socket(54000);
  if (serverSocket == -1) {
    cerr << "failed to start server" << endl;
    return 1;
  }

  eventloop loop;
  loop.run(serverSocket);
  return 0;
}
