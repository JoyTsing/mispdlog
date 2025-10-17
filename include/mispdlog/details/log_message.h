#pragma once

#include "mispdlog/common.h"
#include "mispdlog/level.h"

#include <cstddef>
#include <mispdlog/details/utils.h>
#include <string_view>

namespace mispdlog {
namespace details {
/**
 * @brief  loc
 *
 */
struct source_location {
  // 无动态资源，设计为constexpr编译期可构造
  constexpr source_location() = default;
  constexpr source_location(std::string_view filename, int line,
                            std::string_view function_name)
      : filename(filename), line(line), function_name(function_name) {}

  constexpr bool empty() const noexcept { return line == 0; }

  std::string_view filename{};
  int line{0};
  std::string_view function_name{};
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
  log_message(std::string_view name, mispdlog::level level,
              log_clock::time_point time, source_location loc,
              std::string_view message)
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
  log_message(std::string_view name, mispdlog::level level, source_location loc,
              std::string_view message)
      : log_message(name, level, log_clock::now(), loc, message) {}

  /**
   * @brief Construct a new log message object without other infomation
   *
   * @param name
   * @param level
   * @param message
   */
  log_message(std::string_view name, mispdlog::level level,
              std::string_view message)
      : log_message(name, level, source_location(), message) {}

  log_message(const log_message &) = default;
  log_message &operator=(const log_message &rhs) = default;

  std::string_view logger_name;
  std::string_view payload; // 日志内容
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