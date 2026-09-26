#include "Store.hpp"
#include <optional>
#include <sys/time.h>

#include <sys/time.h>

void store::put(std::string &key, cbObj &obj) { vault[key] = obj; }
std::optional<std::string> store::get(std::string &key) {
  auto it = vault.find(key);
  if (it == vault.end()) {
    return std::nullopt;
  }
  if (it->second.expires_at != cbObj::NO_EXPIRY &&
      now_ms() >= it->second.expires_at) {
    vault.erase(it);
    return std::nullopt;
  }
  return it->second.value;
}
void store::remove(std::string &key) { vault.erase(key); }
bool store::isKey(std::string &key) {
  auto it = vault.find(key);

  // Check if key does NOT exist
  if (it == vault.end()) {
    return false;
  }

  // Expiration check
  if (it->second.expires_at != cbObj::NO_EXPIRY &&
      now_ms() >= it->second.expires_at) {
    vault.erase(it);
    return false;
  }

  return true;
}
