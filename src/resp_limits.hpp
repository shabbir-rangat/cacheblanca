#pragma once

#include <cstddef>

namespace resp {

constexpr std::size_t MAX_BUFFER_SIZE = 1 * 1024 * 1024; // 1 MB
constexpr std::size_t MAX_COMMAND_SIZE = 64 * 1024;      // 64 KB
constexpr std::size_t MAX_BULK_STRING_SIZE = 512 * 1024; // 512 KB
constexpr std::size_t MAX_ARRAY_ELEMENTS = 1024;
constexpr std::size_t MAX_NESTING_DEPTH = 16;

} // namespace resp
