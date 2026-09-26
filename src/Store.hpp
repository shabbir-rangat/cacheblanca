#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <sys/time.h>
#include <unordered_map>
struct cbObj {
  static constexpr uint64_t NO_EXPIRY = -1;
  std::string value;
  uint64_t expires_at = NO_EXPIRY;
};
uint64_t now_ms();

class store {
private:
  std::unordered_map<std::string, cbObj> vault;

public:
  void put(std::string &key, cbObj &object);
  std::optional<std::string> get(std::string &key);
  void remove(std::string &key);
  bool isKey(std::string &key);
};
