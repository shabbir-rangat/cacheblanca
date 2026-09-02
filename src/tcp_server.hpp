#pragma once
#include "eventloop.hpp"
int init_server_socket(int port);
void run_eventloop(int epfd, int serversocket);
void handle_new_connection(int serversocket, eventloop &loop);

void handle_client_read(int clientfd, eventloop &loop);
