#include "mispdlog/pattern_formatter.h"
#include <iterator>

namespace mispdlog {
// Implementation details would go here
// such as flag_formatter derived classes
namespace {

/**
 * @brief A flag formatter that outputs a raw string
 *
 */
class raw_string_formatter : public pattern_formatter::flag_formatter {
public:
  explicit raw_string_formatter(std::string str) : str_(str) {}

  void format(const details::log_message &msg, const std::tm &tm,
              fmt::memory_buffer &buf) override {
    buf.append(str_.data(), str_.data() + str_.size());
  }

  std::unique_ptr<flag_formatter> clone() const override {
    return std::make_unique<raw_string_formatter>(str_);
  }

private:
  std::string str_;
};

/**
 * @brief A flag formatter that outputs the year
 * %Y - 4 digit year
 */
class year_formatter : public pattern_formatter::flag_formatter {
public:
  void format(const details::log_message &msg, const std::tm &tm,
              fmt::memory_buffer &buf) override {
    fmt::format_to(std::back_inserter(buf), "{:04d}", tm.tm_year + 1900);
  }
  std::unique_ptr<flag_formatter> clone() const override {
    return std::make_unique<year_formatter>();
  }
};
/**
 * @brief A flag formatter that outputs the month
 * %m - 2 digit month
 *
 */
class month_formatter : public pattern_formatter::flag_formatter {
public:
  void format(const details::log_message &msg, const std::tm &tm,
              fmt::memory_buffer &buf) override {
    fmt::format_to(std::back_inserter(buf), "{:02d}", tm.tm_mon + 1);
  }
  std::unique_ptr<flag_formatter> clone() const override {
    return std::make_unique<month_formatter>();
  }
};

/**
 * @brief A flag formatter that outputs the day of month
 * %d - 2 digit day of month
 *
 */
class day_formatter : public pattern_formatter::flag_formatter {
public:
  void format(const details::log_message &msg, const std::tm &tm,
              fmt::memory_buffer &buf) override {
    fmt::format_to(std::back_inserter(buf), "{:02d}", tm.tm_mday);
  }
  std::unique_ptr<flag_formatter> clone() const override {
    return std::make_unique<day_formatter>();
  }
};

/**
 * @brief A flag formatter that outputs the hour
 * %H - 2 digit hour (24-hour clock)
 *
 */
class hour_formatter : public pattern_formatter::flag_formatter {
public:
  void format(const details::log_message &msg, const std::tm &tm,
              fmt::memory_buffer &buf) override {
    fmt::format_to(std::back_inserter(buf), "{:02d}", tm.tm_hour);
  }
  std::unique_ptr<flag_formatter> clone() const override {
    return std::make_unique<hour_formatter>();
  }
};

/**
 * @brief A flag formatter that outputs the minute
 * %M - 2 digit minute
 *
 */
class minute_formatter : public pattern_formatter::flag_formatter {
public:
  void format(const details::log_message &msg, const std::tm &tm,
              fmt::memory_buffer &buf) override {
    fmt::format_to(std::back_inserter(buf), "{:02d}", tm.tm_min);
  }
  std::unique_ptr<flag_formatter> clone() const override {
    return std::make_unique<minute_formatter>();
  }
};

/**
 * @brief A flag formatter that outputs the second
 * %S - 2 digit second
 *
 */
class second_formatter : public pattern_formatter::flag_formatter {
public:
  void format(const details::log_message &msg, const std::tm &tm,
              fmt::memory_buffer &buf) override {
    fmt::format_to(std::back_inserter(buf), "{:02d}", tm.tm_sec);
  }
  std::unique_ptr<flag_formatter> clone() const override {
    return std::make_unique<second_formatter>();
  }
};

/**
 * @brief A flag formatter that outputs the log level
 * %l - log level
 *
 */
class level_formatter : public pattern_formatter::flag_formatter {
public:
  void format(const details::log_message &msg, const std::tm &tm,
              fmt::memory_buffer &buf) override {
    std::string_view level_str = level_to_short_string(msg.level);
    buf.append(level_str.data(), level_str.data() + level_str.size());
  }
  std::unique_ptr<flag_formatter> clone() const override {
    return std::make_unique<level_formatter>();
  }
};

/**
 * @brief A flag formatter that outputs the full log level name
 * %L - full log level name
 *
 */
class level_full_formatter : public pattern_formatter::flag_formatter {
public:
  void format(const details::log_message &msg, const std::tm &tm,
              fmt::memory_buffer &buf) override {
    std::string_view level_str = level_to_string(msg.level);
    buf.append(level_str.data(), level_str.data() + level_str.size());
  }
  std::unique_ptr<flag_formatter> clone() const override {
    return std::make_unique<level_full_formatter>();
  }
};

/**
 * @brief  A flag formatter that outputs the log name
 * %n - log name
 *
 */
class name_formatter : public pattern_formatter::flag_formatter {
public:
  void format(const details::log_message &msg, const std::tm &tm,
              fmt::memory_buffer &buf) override {
    buf.append(msg.logger_name.data(),
               msg.logger_name.data() + msg.logger_name.size());
  }
  std::unique_ptr<flag_formatter> clone() const override {
    return std::make_unique<name_formatter>();
  }
};

/**
 * @brief A flag formatter that outputs the log message
 * %v - log message
 *
 */
class payload_formatter : public pattern_formatter::flag_formatter {
public:
  void format(const details::log_message &msg, const std::tm &tm,
              fmt::memory_buffer &buf) override {
    buf.append(msg.payload.data(), msg.payload.data() + msg.payload.size());
  }
  std::unique_ptr<flag_formatter> clone() const override {
    return std::make_unique<payload_formatter>();
  }
};

/**
 * @brief A flag formatter that outputs thread id
 * %t - thread id
 *
 */
class thread_id_formatter : public pattern_formatter::flag_formatter {
public:
  void format(const details::log_message &msg, const std::tm &tm,
              fmt::memory_buffer &buf) override {
    fmt::format_to(std::back_inserter(buf), "{}", msg.thread_id);
  }
  std::unique_ptr<flag_formatter> clone() const override {
    return std::make_unique<thread_id_formatter>();
  }
};
} // namespace

} // namespace mispdlog