#include "mispdlog/pattern_formatter.h"
#include "mispdlog/formatter.h"
#include <chrono>
#include <iterator>
#include <memory>

namespace mispdlog {
// Implementation details would go here
// such as flag_formatter derived classes
namespace {
// 快速整数到字符串转换
inline void fast_uint_to_str(uint64_t n, char *buffer) {
  // 使用查表法优化小数字
  static constexpr char digits_table[] =
      "0001020304050607080910111213141516171819"
      "2021222324252627282930313233343536373839"
      "4041424344454647484950515253545556575859"
      "6061626364656667686970717273747576777879"
      "8081828384858687888990919293949596979899";

  if (n < 100) {
    if (n < 10) {
      buffer[0] = '0' + n;
      buffer[1] = '\0';
    } else {
      const char *d = digits_table + n * 2;
      buffer[0] = d[0];
      buffer[1] = d[1];
      buffer[2] = '\0';
    }
    return;
  }
  // 对于大数字，回退到标准方法
  fmt::format_to(buffer, "{}", n);
}

// 快速两位数转换(用于时间格式化)
inline void fast_two_digits(uint32_t n, char *buffer) {
  if (n < 100) {
    const char *d = &"0001020304050607080910111213141516171819"
                     "2021222324252627282930313233343536373839"
                     "4041424344454647484950515253545556575859"
                     "6061626364656667686970717273747576777879"
                     "8081828384858687888990919293949596979899"[n * 2];
    buffer[0] = d[0];
    buffer[1] = d[1];
  } else {
    buffer[0] = '0';
    buffer[1] = '0';
  }
}

/**
 * @brief aggregate text formatter
 *
 */
class aggregate_formatter : public pattern_formatter::flag_formatter {
public:
  explicit aggregate_formatter(std::string str) : str_(std::move(str)) {}

  void format(const details::log_message &, const std::tm &,
              fmt::memory_buffer &dest) override {
    // 直接内存拷贝，避免逐字符追加
    dest.append(str_.data(), str_.data() + str_.size());
  }

  std::unique_ptr<flag_formatter> clone() const override {
    return std::make_unique<aggregate_formatter>(str_);
  }

  void add_ch(char ch) { str_ += ch; }
  void add_str(const std::string &str) { str_ += str; }

private:
  std::string str_;
};

/**
 * @brief A flag formatter that outputs the year
 * %Y - 4 digit year
 */
class year_formatter : public pattern_formatter::flag_formatter {
public:
  void format([[maybe_unused]] const details::log_message &msg,
              const std::tm &tm, fmt::memory_buffer &buf) override {
    char buffer[5];
    int year = tm.tm_year + 1900;
    // 手动展开避免 sprintf 开销
    buffer[0] = '0' + (year / 1000);
    buffer[1] = '0' + ((year / 100) % 10);
    buffer[2] = '0' + ((year / 10) % 10);
    buffer[3] = '0' + (year % 10);
    buffer[4] = '\0';
    buf.append(buffer, buffer + 4);
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
  void format([[maybe_unused]] const details::log_message &msg,
              const std::tm &tm, fmt::memory_buffer &buf) override {
    char buffer[3];
    fast_two_digits(tm.tm_mon + 1, buffer);
    buf.append(buffer, buffer + 2);
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
  void format([[maybe_unused]] const details::log_message &msg,
              const std::tm &tm, fmt::memory_buffer &buf) override {
    char buffer[3];
    fast_two_digits(tm.tm_mday, buffer);
    buf.append(buffer, buffer + 2);
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
  void format([[maybe_unused]] const details::log_message &msg,
              const std::tm &tm, fmt::memory_buffer &buf) override {
    char buffer[3];
    fast_two_digits(tm.tm_hour, buffer);
    buf.append(buffer, buffer + 2);
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
  void format([[maybe_unused]] const details::log_message &msg,
              const std::tm &tm, fmt::memory_buffer &buf) override {
    char buffer[3];
    fast_two_digits(tm.tm_min, buffer);
    buf.append(buffer, buffer + 2);
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
  void format([[maybe_unused]] const details::log_message &msg,
              const std::tm &tm, fmt::memory_buffer &buf) override {
    char buffer[3];
    fast_two_digits(tm.tm_sec, buffer);
    buf.append(buffer, buffer + 2);
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
  void format(const details::log_message &msg,
              [[maybe_unused]] const std::tm &tm,
              fmt::memory_buffer &buf) override {
    static constexpr std::string_view level_strings[] = {"T", "D", "I",
                                                         "W", "E", "C"};
    if (static_cast<size_t>(msg.level) < std::size(level_strings)) {
      auto level_str = level_strings[static_cast<size_t>(msg.level)];
      buf.append(level_str.data(), level_str.data() + level_str.size());
    }
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
  void format(const details::log_message &msg,
              [[maybe_unused]] const std::tm &tm,
              fmt::memory_buffer &buf) override {
    static constexpr std::string_view level_strings[] = {
        "trace", "debug", "info", "warning", "error", "critical"};

    if (static_cast<size_t>(msg.level) < std::size(level_strings)) {
      auto level_str = level_strings[static_cast<size_t>(msg.level)];
      buf.append(level_str.data(), level_str.data() + level_str.size());
    }
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
  void format(const details::log_message &msg,
              [[maybe_unused]] const std::tm &tm,
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
  void format(const details::log_message &msg,
              [[maybe_unused]] const std::tm &tm,
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
  void format(const details::log_message &msg,
              [[maybe_unused]] const std::tm &tm,
              fmt::memory_buffer &buf) override {
    char buffer[64];
    fast_uint_to_str(static_cast<uint64_t>(msg.thread_id), buffer);
    size_t len = std::strlen(buffer);
    buf.append(buffer, buffer + len);
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
  // 预分配空间避免多次重新分配
  buf.reserve(buf.size() + 256);

  // update tm when seconds change
  auto secs = std::chrono::duration_cast<std::chrono::seconds>(
      msg.time.time_since_epoch());
  if (secs != last_log_seconds_) {
    last_log_seconds_ = secs;
    cached_tm_ = get_time(msg);
  }

  for (const auto &formatter : formatters_) {
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
  std::unique_ptr<aggregate_formatter> raw_str;
  while (it != end) {
    if (*it != '%') {
      if (!raw_str) {
        raw_str = std::make_unique<aggregate_formatter>("");
      }
      raw_str->add_ch(*it);
      ++it;
    } else {
      if (raw_str) {
        formatters_.emplace_back(std::move(raw_str));
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
          if (!raw_str) {
            raw_str = std::make_unique<aggregate_formatter>("");
          }
          raw_str->add_ch('%');
          break;
        default:
          // unknown flag, treat as raw string
          if (!raw_str) {
            raw_str = std::make_unique<aggregate_formatter>("");
          }
          raw_str->add_ch('%');
          raw_str->add_ch(flag);
          break;
        }
      }
    }
  } // while
  if (raw_str) {
    formatters_.emplace_back((std::move(raw_str)));
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