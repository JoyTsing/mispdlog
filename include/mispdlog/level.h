#pragma once

#include <cstdint>
namespace mispdlog {
// 有宏展开的写法，见threadpool中的minilog
enum class level : std::uint8_t {
  trace = 0,    // 最详细的调试信息
  debug = 1,    // 调试信息
  info = 2,     // 普通信息
  warn = 3,     // 警告信息
  error = 4,    // 错误信息
  critical = 5, // 严重错误
  off = 6       // 关闭日志
};
} // namespace mispdlog