#pragma once

#include "mispdlog/details/log_message.h"
#include "mispdlog/formatter.h"
#include <chrono>
#include <ctime>
#include <fmt/format.h>
#include <vector>
namespace mispdlog {

class pattern_formatter : public formatter {
public:
  /**
   * @brief Construct a new pattern formatter object
   *
   * @param pattern
   */
  explicit pattern_formatter(
      const std::string &pattern = "[%Y-%m-%d %H:%M:%S][%L]%v");

  ~pattern_formatter() override = default;

  /**
   * @brief Format the log message according to the pattern;
   * Pattern: [%Y-%m-%d %H:%M:%S] [%l] %v ;
   * Output:  [2025-09-30 03:36:39] [I] Hello, World!
   * @param msg
   * @param buf
   */
  void format(const details::log_message &msg,
              fmt::memory_buffer &buf) override;

  std::unique_ptr<formatter> clone() const override;

  /**
   * @brief Set the pattern object
   *
   * @param pattern
   */
  void set_pattern(const std::string &pattern);

public:
  /**
   * @brief P-IMPL, Base class for flag formatters, each flag formatter is
   * responsible for formatting a specific part of the log message, e.g.,
   * timestamp, level, message, etc.
   *
   */
  class flag_formatter {
  public:
    virtual ~flag_formatter() = default;

    /**
     * @brief Format the specific part of the log message
     *
     * @param msg
     * @param tm
     * @param buf
     */
    virtual void format(const details::log_message &msg, const std::tm &tm,
                        fmt::memory_buffer &buf) = 0;

    /**
     * @brief Clone the flag formatter
     *
     * @return std::unique_ptr<flag_formatter>
     */
    virtual std::unique_ptr<flag_formatter> clone() const = 0;
  };

private:
  void compile_pattern();

  std::tm get_time(const details::log_message &msg) const;

private:
  std::string pattern_;
  std::vector<std::unique_ptr<flag_formatter>> formatters_;

  // 缓存上次格式化的时间，优化时间格式化性能
  std::chrono::seconds last_log_seconds_{0};
  std::tm cached_tm_{};
};
} // namespace mispdlog