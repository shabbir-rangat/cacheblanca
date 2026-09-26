#pragma once
#include "client.hpp"
#include "command.hpp"
#include "eventloop.hpp"
#include <optional>
#include <string>
#include <vector>
// take the bytes from the clients read buffer and decode it and put it in
// clients write buffer.
enum class respType { INTEGER, SIMPLESTRING, BULKSTRING, ERROR, ARRAY };

enum class parserResult {
  COMPLETE,
  INCOMPLETE,
  ERROR,
};

parserResult decode_command(std::string_view buffer, size_t &consumed,
                            command &command);
void parseBulkString();
void parseArray();
