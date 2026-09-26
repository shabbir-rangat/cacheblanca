#pragma once
#include <string>

struct Client {
  int fd;
  std::string read_buffer;
  std::string write_buffer;
};
