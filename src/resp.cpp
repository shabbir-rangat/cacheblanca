#include "resp.hpp"
#include "Store.hpp"
#include "utils.hpp"
#include <climits>
#include <cstdint>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>
bool is_uint64(const std::string &str) {
  if (str.empty())
    return false;

  // Disallow negative sign explicitly because stoull might accept '-' and wrap
  // around
  if (str[0] == '-')
    return false;

  try {
    size_t idx = 0;
    // std::stoull converts string to unsigned long long
    unsigned long long val = std::stoull(str, &idx, 10);

    // Ensure the entire string was consumed and it fits in uint64_t
    return idx == str.length() && val <= UINT64_MAX;
  } catch (const std::out_of_range &) {
    // Number is too large for unsigned long long / uint64_t
    return false;
  } catch (const std::invalid_argument &) {
    // No conversion could be performed
    return false;
  }
}
void evalDEL(std::string &key, store &myStore, std::string &outbuff) {
  // on deleteing a key which deosnt exist return 0
  // on deleting succesfully return 1
  if (!myStore.isKey(key)) {
    outbuff = outbuff + ":0\r\n";
  } else {
    myStore.remove(key);
    outbuff += ":1\r\n";
  }
}
void evalSET(std::vector<std::string> &args, store &myStore,
             std::string &outbuff) {
  if (args.size() < 2) {

    outbuff += "-ERR wrong number of arguments for 'set' command\r\n";
    return;
  }
  std::string key;
  std::string value;
  uint64_t exDurationMs = -1;
  key = args[0];
  value = args[1];
  uint64_t expiresAt = cbObj::NO_EXPIRY;
  for (int i = 2; i < args.size(); ++i) {
    if (args[i] == "EX" || args[i] == "ex") {
      i++;
      if (i == args.size()) {

        outbuff += "-ERR syntax error\r\n";
        return;
      }
      if (!is_uint64(args[i])) {
        outbuff += "(error) value is not an integer or is out of range";
        return;

      } else {
        exDurationMs = std::stoull(args[i]);
      }
      uint64_t exDurationSec = std::stoull(args[i]);
      expiresAt = now_ms() + exDurationSec * 1000;

    } else {

      outbuff += "-ERR syntax error\r\n";
      return;
    }
  }
  cbObj obj;
  obj.value = value;
  obj.expires_at = expiresAt;

  myStore.put(key, obj);
  outbuff += "+OK\r\n";
}
void evalGET(std::vector<std::string> &args, store &myStore,
             std::string &outbuff) {
  if (args.size() != 1) {
    outbuff += "ERR- wrong number of arguemnts\r\n";
    return;
  }
  std::string key = args[0];
  auto result = myStore.get(key);
  if (!result.has_value()) {
    outbuff += "$-1\r\n";
    return;
  }

  outbuff += "$" + std::to_string(result->size()) + "\r\n" + *result + "\r\n";
};
