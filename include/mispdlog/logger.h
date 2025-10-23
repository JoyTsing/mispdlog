#pragma once

#include "mispdlog/common.h"
#include "mispdlog/details/log_message.h"
#include "mispdlog/level.h"
#include "mispdlog/sinks/base_sink.h"
#include <fmt/core.h>
#include <fmt/format.h>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

namespace mispdlog {
class MISPDLOG_API logger {
public:
  explicit logger(std::string nmae);
  logger(std::string name, sinks::sink_ptr single_sink);
  logger(std::string name, std::vector<sinks::sink_ptr> sinks);

  virtual ~logger() = default;

  // no copy
  logger(const logger &) = delete;
  logger &operator=(const logger &) = delete;

public:
  void add_sink(sinks::sink_ptr sink);
  void remove_sink(sinks::sink_ptr sink);
  std::vector<sinks::sink_ptr> &sinks();
  const std::vector<sinks::sink_ptr> &sinks() const;

  void set_level(level level);
  level get_level() const;
  bool should_log(level message_level) const;

  void flush();
  void flush_when(level level);

  const std::string &name() const;

public:
  /**
   * @brief log output
   *
   * @tparam Args
   * @param level
   * @param fmt
   * @param args
   */
  template <typename... Args>
  void log(level level, fmt::format_string<Args...> fmt, Args &&...args) {
    if (should_log(level) == false) {
      return;
    }
    fmt::memory_buffer buf;
    fmt::format_to(std::back_inserter(buf), fmt, std::forward<Args>(args)...);
    // log_message
    details::log_message message(name_, level,
                                 string_view_t(buf.data(), buf.size()));
    // sink it
    sink_it_(message);
  }

  /**
   * @brief trace log output
   *
   * @tparam Args
   * @param fmt
   * @param args
   */
  template <typename... Args>
  void trace(fmt::format_string<Args...> fmt, Args &&...args) {
    log(level::trace, fmt, args...);
  }

  /**
   * @brief debug output
   *
   * @tparam Args
   * @param fmt
   * @param args
   */
  template <typename... Args>
  void debug(fmt::format_string<Args...> fmt, Args &&...args) {
    log(level::debug, fmt, args...);
  }

  /**
   * @brief info output
   *
   * @tparam Args
   * @param fmt
   * @param args
   */
  template <typename... Args>
  void info(fmt::format_string<Args...> fmt, Args &&...args) {
    log(level::info, fmt, args...);
  }

  /**
   * @brief warn output
   *
   * @tparam Args
   * @param fmt
   * @param args
   */
  template <typename... Args>
  void warn(fmt::format_string<Args...> fmt, Args &&...args) {
    log(level::warn, fmt, args...);
  }

  /**
   * @brief error output
   *
   * @tparam Args
   * @param fmt
   * @param args
   */
  template <typename... Args>
  void error(fmt::format_string<Args...> fmt, Args &&...args) {
    log(level::error, fmt, args...);
  }

  /**
   * @brief critical output
   *
   * @tparam Args
   * @param fmt
   * @param args
   */
  template <typename... Args>
  void critical(fmt::format_string<Args...> fmt, Args &&...args) {
    log(level::critical, fmt, args...);
  }

  /**
   * @brief off output
   *
   * @tparam Args
   * @param fmt
   * @param args
   */
  template <typename... Args>
  void off(fmt::format_string<Args...> fmt, Args &&...args) {
    log(level::off, fmt, args...);
  }

protected:
  /**
   * @brief through sink to real output
   *
   * @param message
   */
  virtual void sink_it_(const details::log_message &message);

protected:
  std::string name_;
  std::vector<sinks::sink_ptr> sinks_;
  level level_{level::trace};
  level flush_level_{level::off}; // 自动刷新日志等级
};
} // namespace mispdlog