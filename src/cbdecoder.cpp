#include "cbdecoder.hpp"
#include <charconv>
#include <execution>
#include <iostream>
#include <optional>
#include <string_view>
#include <system_error>
#include <vector>
std::optional<int> str_to_int(std::string_view str) {
  if (str.empty()) {
    return std::nullopt;
  }
  int result = 0;
  auto res = std::from_chars(str.data(), str.data() + str.size(), result);
  if (res.ec == std::errc() && res.ptr == str.data() + str.size()) {
    return result;
  }
  return std::nullopt;
}
parserResult decode_command(std::string_view buffer, size_t &consumed,
                            command &command) {
  if (buffer.empty()) {
    return parserResult::INCOMPLETE;
  }
  std::cerr << "DEBUG: raw buffer=[" << buffer << "] size=" << buffer.size()
            << "\n";

  if (buffer[0] != '*') {
    std::cerr << "expected '*' got " << buffer << std::endl;
    return parserResult::ERROR;
  }
  // find crlf (\r\n) extract the char before it and convert it into a number
  // using from_chars
  size_t crlf_position = buffer.find("\r\n");
  if (crlf_position == std::string_view::npos) {
    return parserResult::INCOMPLETE;
  }
  // extracting the number before \r\n
  std::string_view num_str = buffer.substr(1, crlf_position - 1);
  int size_of_array = 0;
  auto result = std::from_chars(num_str.data(), num_str.data() + num_str.size(),
                                size_of_array);
  if (result.ec != std::errc()) {
    return parserResult::ERROR;
  }
  size_t current_idx = crlf_position + 2;
  // looping till size_of_array extracting bulk strigs of exaclty size_of_array.
  for (int i = 0; i < size_of_array; ++i) {
    if (current_idx >= buffer.size()) {
      return parserResult::INCOMPLETE;
    }
    if (buffer[current_idx] != '$') {
      std::cerr << "DEBUG: expected '$' at idx=" << current_idx
                << ", got buffer=[" << buffer << "]\n";
      return parserResult::ERROR;
    }
    // find crlf and extract the length of the bulk string;
    size_t next_crlf = buffer.find("\r\n", current_idx);
    if (next_crlf == std::string_view::npos) {
      return parserResult::INCOMPLETE;
    }
    std::string_view num_str =
        buffer.substr(current_idx + 1, next_crlf - current_idx - 1);
    auto length_of_bulkstring = str_to_int(num_str);
    if (!length_of_bulkstring.has_value()) {
      std::cerr << "DEBUG: failed to parse bulk length from num_str=["
                << num_str << "]\n";
      return parserResult::ERROR;
    }
    if (*length_of_bulkstring < 0) {
      std::cerr << "DEBUG: negative bulk length: " << *length_of_bulkstring
                << "\n";
      return parserResult::ERROR;
    }
    size_t paylod_start = next_crlf + 2;
    // casted optional<int> to size_t to calculate total_string_block;
    size_t bulk_len = static_cast<size_t>(*length_of_bulkstring);

    // this will give us the expected number of bytes in a string block and will
    // give us the way to the next token;
    size_t total_string_block = (paylod_start - current_idx) + bulk_len + 2;
    if (total_string_block + current_idx > buffer.size()) {
      return parserResult::INCOMPLETE;
    }
    // putting the argument in the  command.args
    std::string_view arg = buffer.substr(paylod_start, bulk_len);
    // checking crlf after the payload ;
    if (buffer.substr(paylod_start + bulk_len, 2) != "\r\n") {
      std::cerr << "DEBUG: missing trailing crlf after payload, got=["
                << buffer.substr(paylod_start + bulk_len, 2) << "]\n";
      return parserResult::ERROR;
    }
    command.args.push_back(std::string(arg));
    current_idx = current_idx + total_string_block;
  }

  consumed = current_idx;
  std::cerr << "DEBUG: reached COMPLETE, consumed=" << consumed << "\n";
  return parserResult::COMPLETE;
}
