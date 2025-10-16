#include <cstddef>
#include <cstdint>
#include <mispdlog/common.h>
#include <mispdlog/level.h>
#include <string>
#include <string_view>

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <pthread.h>
#else
#include <thread>
#endif
namespace mispdlog {

namespace details {
// 上色
#if defined(__linux__) || defined(__APPLE__)
inline constexpr char _k_level_ansi_colors[(std::uint8_t)level::off + 1][8] = {
    "\x1B[37m", "\x1B[35m", "\x1B[32m",  "\x1B[34m",
    "\x1B[33m", "\x1B[31m", "\x1B[31;1m"};
inline constexpr char _k_reset_ansi_color[4] = "\x1B[m";
#define _LOG_IF_HAS_ANSI_COLORS(x) x
#else
#define _LOG_IF_HAS_ANSI_COLORS(x)
inline constexpr char _k_level_ansi_colors[(std::uint8_t)level::off + 1][1] = {
    "", "", "", "", "", "", "",
};
inline constexpr char _k_reset_ansi_color[1] = "";
#endif

#define COLORFUL_STRING(level, msg)                                            \
  _LOG_IF_HAS_ANSI_COLORS(k_level_ansi_colors[(std::uint8_t)level] +)          \
  msg _LOG_IF_HAS_ANSI_COLORS(+k_reset_ansi_color)

// format time
MISPDLOG_API std::string
format_time(const log_clock::time_point &tp,
            std::string_view format = "%Y-%m-%d %H:%M:%S");

// 获取当前时间戳(毫秒)
MISPDLOG_API uint64_t get_timestamp_ms();

MISPDLOG_API size_t get_thread_id();

// utils
MISPDLOG_API std::string &left_trim(std::string &s);
MISPDLOG_API std::string &right_trim(std::string &s);
MISPDLOG_API std::string &trim(std::string &s);
} // namespace details
} // namespace mispdlog