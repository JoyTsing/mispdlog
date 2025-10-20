#pragma once

#include "mispdlog/details/log_message.h"
#include <fmt/format.h>
#include <memory>

namespace mispdlog {

/**
 * @brief Base class for log message formatters,every sink should have one
 *
 */
class formatter {
public:
  virtual ~formatter() = default;

  /**
   * @brief Format the log message into the provided buffer, for best
   * performance
   *
   * @param msg
   * @param buf
   */
  virtual void format(const details::log_message &msg,
                      fmt::memory_buffer &buf) = 0;

  /**
   * @brief Clone the formatter,to avoid multithreading issues
   *
   * @return std::unique_ptr<formatter>
   */
  virtual std::unique_ptr<formatter> clone() const = 0;
};
} // namespace mispdlog