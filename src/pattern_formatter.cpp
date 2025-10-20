#include "mispdlog/pattern_formatter.h"
#include "mispdlog/formatter.h"
#include <chrono>
#include <iterator>
#include <memory>

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

pattern_formatter::pattern_formatter(const std::string &pattern)
    : pattern_(pattern) {
  compile_pattern();
}

void pattern_formatter::format(const details::log_message &msg,
                               fmt::memory_buffer &buf) {
  // update tm when seconds change
  auto secs = std::chrono::duration_cast<std::chrono::seconds>(
      msg.time.time_since_epoch());
  if (secs != last_log_seconds_) {
    last_log_seconds_ = secs;
    cached_tm_ = get_time(msg);
  }

  for (auto &formatter : formatters_) {
    formatter->format(msg, cached_tm_, buf);
  }
  buf.push_back('\n');
}

std::unique_ptr<formatter> pattern_formatter::clone() const {
  return std::make_unique<pattern_formatter>(pattern_);
}

void pattern_formatter::set_pattern(const std::string &pattern) {
  pattern_ = pattern;
  formatters_.clear();
  compile_pattern();
}

void pattern_formatter::compile_pattern() {
  auto it = pattern_.begin();
  auto end = pattern_.end();
  // parse the pattern
  std::string raw_str;
  while (it != end) {
    if (*it != '%') {
      raw_str.push_back(*it);
      ++it;
    } else {
      if (!raw_str.empty()) {
        formatters_.emplace_back(
            std::make_unique<raw_string_formatter>(std::move(raw_str)));
        raw_str.clear();
      }
      ++it; // skip '%'
      if (it != end) {
        char flag = *it;
        ++it;
        // create corresponding flag formatter
        switch (flag) {
        case 'Y':
          formatters_.emplace_back(std::make_unique<year_formatter>());
          break;
        case 'm':
          formatters_.emplace_back(std::make_unique<month_formatter>());
          break;
        case 'd':
          formatters_.emplace_back(std::make_unique<day_formatter>());
          break;
        case 'H':
          formatters_.emplace_back(std::make_unique<hour_formatter>());
          break;
        case 'M':
          formatters_.emplace_back(std::make_unique<minute_formatter>());
          break;
        case 'S':
          formatters_.emplace_back(std::make_unique<second_formatter>());
          break;
        case 'l':
          formatters_.emplace_back(std::make_unique<level_formatter>());
          break;
        case 'L':
          formatters_.emplace_back(std::make_unique<level_full_formatter>());
          break;
        case 'n':
          formatters_.emplace_back(std::make_unique<name_formatter>());
          break;
        case 'v':
          formatters_.emplace_back(std::make_unique<payload_formatter>());
          break;
        case 't':
          formatters_.emplace_back(std::make_unique<thread_id_formatter>());
          break;
        case '%':
          // escaped '%'
          raw_str.push_back('%');
          break;
        default:
          // unknown flag, treat as raw string
          raw_str.push_back('%' + flag);
          break;
        }
      }
    }
  } // while
  if (!raw_str.empty()) {
    formatters_.emplace_back(
        std::make_unique<raw_string_formatter>(std::move(raw_str)));
  }
}

std::tm pattern_formatter::get_time(const details::log_message &msg) const {
  auto in_time_t = log_clock::to_time_t(msg.time);
  std::tm tm;
#ifdef _WIN32
  localtime_s(&tm, &in_time_t);
#else
  localtime_r(&in_time_t, &tm);
#endif
  return tm;
}

} // namespace mispdlog