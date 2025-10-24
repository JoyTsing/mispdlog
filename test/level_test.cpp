#define ANKERL_NANOBENCH_IMPLEMENT
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "mispdlog/details/utils.h"
#include "mispdlog/level.h"

#include <doctest.h>
#include <nanobench.h>

using namespace mispdlog;

// NOLINTNEXTLINE
TEST_CASE("test level conversion") { // 测试所有级别
  CHECK_NOTHROW(
      level levels[] = {level::trace, level::debug, level::info, level::warn,
                        level::error, level::critical, level::off};
      for (auto lvl
           : levels) {
        std::cout << "Level: " << std::setw(10) << level_to_string(lvl)
                  << " | Short: " << level_to_short_string(lvl)
                  << " | Value: " << static_cast<int>(lvl) << "\n";
      });
}

// NOLINTNEXTLINE
TEST_CASE("test string to level") {
  CHECK_NOTHROW(std::cout << '\n';
                std::string test_strings[] = {"trace", "DEBUG", "Info", "WARN",
                                              "unknown"};

                for (const auto &str
                     : test_strings) {
                  auto lvl = string_to_level(str);
                  std::cout << "Input: " << std::setw(10) << str << " -> "
                            << level_to_string(lvl) << "\n";
                });
}

// NOLINTNEXTLINE
TEST_CASE("test should log") {
  CHECK_NOTHROW(std::cout << '\n'; auto logger_level = level::info;
                level test_levels[] = {level::trace, level::debug, level::info,
                                       level::warn, level::error};
                bool check[] = {false, false, true, true, true}; int index = 0;
                std::cout << "Logger level set to: "
                          << level_to_string(logger_level) << "\n";
                for (auto msg_level
                     : test_levels) {
                  bool should = should_log(logger_level, msg_level);
                  CHECK_EQ(check[index++], should);
                  std::cout << "Message level: " << std::setw(10)
                            << level_to_string(msg_level) << " -> "
                            << (should ? "✓ 输出" : "✗ 过滤") << "\n";
                });
}

// NOLINTNEXTLINE
TEST_CASE("test time utils") {
  CHECK_NOTHROW( // 直接使用 log_clock
      std::cout << '\n'; auto now = mispdlog::log_clock::now();

      std::cout << "当前时间(默认格式): " << details::format_time(now) << "\n";
      std::cout << "当前时间(自定义):   "
                << details::format_time(now, "%Y年%m月%d日 %H:%M:%S") << "\n";
      std::cout << "时间戳(毫秒):       " << details::get_timestamp_ms()
                << "\n";);
}

// NOLINTNEXTLINE
TEST_CASE("test string utils") {
  CHECK_NOTHROW(
      std::cout << '\n'; std::string test1 = "  hello  ";
      std::string test2 = test1; std::string test3 = test1;

      std::cout << "原始字符串: [" << test1 << "]\n";
      std::cout << "ltrim:      [" << details::left_trim(test1) << "]\n";
      std::cout << "rtrim:      [" << details::right_trim(test2) << "]\n";
      std::cout << "trim:       [" << details::trim(test3) << "]\n";);
}
// NOLINTNEXTLINE
TEST_CASE("test colorful") {
  CHECK_NOTHROW(
      std::cout << '\n'
                << "test color" << '\n';
      level levels[] = {level::trace, level::debug, level::info, level::warn,
                        level::error, level::critical, level::off};
      for (auto level
           : levels) {
        std::cout << details::color(level, level_to_string(level)) << "\n";
      } std::cout
      << "test color back normal" << '\n';);
}
