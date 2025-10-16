#pragma once

#include "mispdlog/common.h"
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

MISPDLOG_API std::string_view level_to_string(level lv) noexcept;
MISPDLOG_API std::string_view level_to_short_string(level lv) noexcept;
MISPDLOG_API level string_to_level(const std::string &str);

// 级别比较函数
inline bool should_log(level logger_level, level msg_level) noexcept {
  return msg_level >= logger_level;
}
} // namespace mispdlog