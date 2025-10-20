#pragma once

#include "mispdlog/common.h"
#include "mispdlog/level.h"

#include <cstddef>
#include <mispdlog/details/utils.h>

namespace mispdlog {
namespace details {
/**
 * @brief  loc
 *
 */
struct source_location {
  // 无动态资源，设计为constexpr编译期可构造
  constexpr source_location() = default;
  constexpr source_location(const char *filename, int line,
                            const char *function_name)
      : filename(filename), line(line), function_name(function_name) {}

  constexpr bool empty() const noexcept { return line == 0; }

  const char *filename{nullptr};
  int line{0};
  const char *function_name{nullptr};
};

/**
 * @brief
 *
 */
struct log_message {
  log_message() = default;

  /**
   * @brief Construct a new log message object fully
   *
   * @param name
   * @param level
   * @param time
   * @param loc
   * @param message
   */
  log_message(string_view_t name, mispdlog::level level,
              log_clock::time_point time, source_location loc,
              string_view_t message)
      : logger_name(name), payload(message), level(level), time(time), loc(loc),
        thread_id(get_thread_id()) {}

  /**
   * @brief Construct a new log message object quickly
   *
   * @param name
   * @param level
   * @param loc
   * @param message
   */
  log_message(string_view_t name, mispdlog::level level, source_location loc,
              string_view_t message)
      : log_message(name, level, log_clock::now(), loc, message) {}

  /**
   * @brief Construct a new log message object without other infomation
   *
   * @param name
   * @param level
   * @param message
   */
  log_message(string_view_t name, mispdlog::level level, string_view_t message)
      : log_message(name, level, source_location(), message) {}

  log_message(const log_message &) = default;
  log_message &operator=(const log_message &rhs) = default;

  string_view_t logger_name;
  string_view_t payload; // 日志内容
  mispdlog::level level{mispdlog::level::info};
  log_clock::time_point time; // timestamp
  source_location loc;
  size_t thread_id{0};

  // 颜色范围(用于格式化时着色,由 formatter 设置)
  mutable size_t color_range_start{0};
  mutable size_t color_range_end{0};
};
} // namespace details
} // namespace mispdlog