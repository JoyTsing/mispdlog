#include "mispdlog/common.h"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <mispdlog/details/utils.h>
#include <sstream>
namespace mispdlog {
namespace details {

MISPDLOG_API std::string format_time(const log_clock::time_point &tp,
                                     std::string_view format) {
  std::time_t time_t_val = log_clock::to_time_t(tp);
  std::tm tm_val;
  // 线程安全的时间转换
#ifdef _WIN32
  localtime_s(&tm_val, &time_t_val);
#else
  localtime_r(&time_t_val, &tm_val);
#endif
  std::ostringstream oss;
  oss << std::put_time(&tm_val, format.data());
  return oss.str();
}

uint64_t get_timestamp_ms() {
  std::chrono::time_point now = log_clock::now();
  std::chrono::duration duration = now.time_since_epoch();
  return std::chrono::duration_cast<std::chrono::milliseconds>(duration)
      .count();
}

size_t get_thread_id() {
#ifdef _WIN32
  return static_cast<size_t>(::GetCurrentThreadId());
#elif defined(__linux__) || defined(__APPLE__)
  return static_cast<size_t>(pthread_self());
#else
  std::hash<std::thread::id> hasher;
  return hasher(std::this_thread::get_id());
#endif
}

std::string &left_trim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
          }));
  return s;
}

std::string &right_trim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [](unsigned char ch) { return !std::isspace(ch); })
              .base(),
          s.end());
  return s;
}

std::string &trim(std::string &s) { return left_trim(right_trim(s)); }
} // namespace details
} // namespace mispdlog