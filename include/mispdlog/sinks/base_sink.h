#pragma once

#include "mispdlog/details/log_message.h"
#include "mispdlog/level.h"
#include <mutex>

namespace mispdlog {
namespace sinks {

struct null_mutex {
  void lock() {}
  void unlock() {}
};

class sink {
public:
  virtual ~sink() = default;

  /**
   * @brief base log message
   *
   * @param msg
   */
  virtual void log(const details::log_message &msg) = 0;

  /**
   * @brief flush cache
   *
   */
  virtual void flush() = 0;

  virtual void set_level(level log_level) = 0;
  virtual level get_level() const = 0;

  virtual bool should_log(level message_level) const = 0;
};

/**
 * @brief base sink
 *
 * @tparam Mutex
 */
template <typename Mutex> class base_sink : public sink {
public:
  base_sink() : level_(level::info) {}
  virtual ~base_sink() = default;

  /**
   * @brief forbidden construct & copy
   *
   */
  base_sink(base_sink &&) = delete;

  void log(const details::log_message &msg) override {
    std::lock_guard<Mutex> lock(mutex_);
    if (should_log(msg.level)) {
      sink_it_(msg);
    }
  }

  void flush() override {
    std::lock_guard<Mutex> lock(mutex_);
    flush_();
  }

  void set_level(level log_level) override {
    std::lock_guard<Mutex> lock(mutex_);
    level_ = log_level;
  }

  level get_level() const override {
    std::lock_guard<Mutex> lock(mutex_);
    return level_;
  }

  bool should_log(level message_level) const override {
    return message_level >= level_;
  }

protected:
  virtual void sink_it_(const details::log_message &msg) = 0;
  virtual void flush_() = 0;

protected:
  mutable Mutex mutex_;
  level level_;
};
} // namespace sinks
} // namespace mispdlog
