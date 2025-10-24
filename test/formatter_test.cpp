#define ANKERL_NANOBENCH_IMPLEMENT
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "mispdlog/level.h"
#include "mispdlog/pattern_formatter.h"
#include "mispdlog/sinks/console_sink.h"

#include <doctest.h>
#include <fmt/format.h>
#include <memory>
#include <nanobench.h>
#include <thread>

using namespace mispdlog;
// NOLINTNEXTLINE
TEST_CASE("test_pattern_compilation") {
  std::cout << "\n========== 测试1:Pattern 编译 ==========\n";
  // 创建自定义 pattern
  std::string patterns[] = {"[%Y-%m-%d %H:%M:%S] [%l] %v", "%H:%M:%S.%L - %v",
                            "[%L] [%n] [thread %t] %v",
                            "%Y年%m月%d日 %H时%M分%S秒 %v"};
  CHECK_NOTHROW(for (const auto &pattern
                     : patterns) {
    pattern_formatter formatter(pattern);
    fmt::memory_buffer buf;

    details::log_message msg("TestLogger", level::info, "Hello, World!");
    formatter.format(msg, buf);

    std::cout << "Pattern: " << pattern << "\n";
    std::cout << "Output:  " << std::string(buf.data(), buf.size());
    std::cout << "\n";
  });
}

// NOLINTNEXTLINE
TEST_CASE("test_all_flags") {
  std::cout << "\n========== 测试2:所有占位符 ==========\n";
  CHECK_NOTHROW(
      pattern_formatter formatter(
          "Year:%Y Month:%m Day:%d Hour:%H Min:%M Sec:%S "
          "Level:%l(%L) Name:%n Thread:%t Msg:%v");

      details::log_message msg("MyLogger", level::warn, "Test message");
      fmt::memory_buffer buf; formatter.format(msg, buf);

      std::cout << std::string(buf.data(), buf.size()););
}

// NOLINTNEXTLINE
TEST_CASE("test_level_formatting()") {
  std::cout << "\n========== 测试3:不同级别格式化 ==========\n";
  auto sink = std::make_shared<sinks::console_sink_mt>();
  sink->set_level(level::trace);
  CHECK_NOTHROW( // 设置自定义 pattern
      sink->set_formatter(
          std::make_unique<pattern_formatter>("[%Y-%m-%d %H:%M:%S] [%L] %v"));

      level levels[] = {level::trace, level::debug, level::info, level::warn,
                        level::error, level::critical};

      for (auto lvl
           : levels) {
        std::string msg_text = std::string("This is ") +
                               fmt::to_string(level_to_string(lvl)) +
                               " message";
        details::log_message msg("TestLogger", lvl, msg_text);
        sink->log(msg);
      });
}

// NOLINTNEXTLINE
TEST_CASE("test_time_caching") {
  std::cout << "\n========== 测试4:时间缓存性能 ==========\n";

  CHECK_NOTHROW(
      pattern_formatter formatter("[%Y-%m-%d %H:%M:%S] [%l] %v");

      // 在同一秒内记录多条日志,应该复用缓存的时间
      auto start = std::chrono::high_resolution_clock::now();

      const int iterations = 10000;
      for (int i = 0; i < iterations; ++i) {
        details::log_message msg("PerfTest", level::info, "Test message");
        fmt::memory_buffer buf;
        formatter.format(msg, buf);
      }

      auto end = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start);

      std::cout << "格式化 " << iterations
                << " 条日志耗时: " << duration.count() << " 微秒\n";
      std::cout << "平均每条: " << (duration.count() / (double)iterations)
                << " 微秒\n";
      std::cout << "\n 时间缓存机制:\n";
      std::cout << "  - 同一秒内的日志消息共享相同的 tm 结构\n";
      std::cout << "  - 避免重复调用 localtime_r/localtime_s\n";
      std::cout << "  - 显著提升性能(特别是高频日志场景)\n";);
}

// NOLINTNEXTLINE
TEST_CASE("test_custom_patterns") {
  std::cout << "\n========== 测试5:自定义 Pattern ==========\n";
  struct PatternTest {
    std::string pattern;
    std::string description;
  };
  CHECK_NOTHROW(
      PatternTest tests[] = {{"[%H:%M:%S] %v", "仅时间 + 消息"},
                             {"%l | %v", "仅级别 + 消息"},
                             {"[%n] %v", "仅 Logger 名称 + 消息"},
                             {"%Y%m%d %H%M%S [%L] %v", "紧凑格式"},
                             {"[%Y-%m-%d %H:%M:%S] [%n] [%L] [tid:%t] %v",
                              "完整格式"}};

      for (const auto &test
           : tests) {
        pattern_formatter formatter(test.pattern);
        details::log_message msg("CustomLogger", level::info,
                                 "Sample log message");
        fmt::memory_buffer buf;
        formatter.format(msg, buf);

        std::cout << "描述: " << test.description << "\n";
        std::cout << "输出: " << std::string(buf.data(), buf.size());
      });
}

// NOLINTNEXTLINE
TEST_CASE("test_escape_percent") {
  std::cout << "\n========== 测试6:百分号转义 ==========\n";
  CHECK_NOTHROW(
      pattern_formatter formatter("Progress: 50%% - %v");
      details::log_message msg("TestLogger", level::info, "Task completed");
      fmt::memory_buffer buf; formatter.format(msg, buf);

      std::cout << "Pattern: Progress: 50%% - %v\n";
      std::cout << "Output:  " << std::string(buf.data(), buf.size()););
}

// NOLINTNEXTLINE
TEST_CASE("test_formatter_in_sink") {
  std::cout << "\n========== 测试7:Sink 中的 Formatter ==========\n";

  CHECK_NOTHROW(auto sink1 = std::make_shared<sinks::console_sink_mt>();
                auto sink2 = std::make_shared<sinks::console_sink_mt>();

                // 为不同 sink 设置不同格式
                sink1->set_formatter(std::make_unique<pattern_formatter>(
                    "[简洁] [%H:%M:%S] %v"));

                sink2->set_formatter(std::make_unique<pattern_formatter>(
                    "[详细] [%Y-%m-%d %H:%M:%S] [%L] [%n] %v"));

                details::log_message msg("MultiSinkTest", level::info,
                                         "Testing different formats");

                std::cout << "Sink 1 输出:\n"; sink1->log(msg);

                std::cout << "Sink 2 输出:\n"; sink2->log(msg););
}

// NOLINTNEXTLINE
TEST_CASE("test_pattern_change") {
  std::cout << "\n========== 测试8:动态修改 Pattern ==========\n";
  CHECK_NOTHROW(
      auto sink = std::make_shared<sinks::console_sink_mt>();
      details::log_message msg("DynamicTest", level::info, "Same message");

      // 第一次输出
      std::cout << "Pattern 1: [默认]\n"; sink->log(msg);

      // 修改 pattern
      sink->set_formatter(
          std::make_unique<pattern_formatter>("[%H:%M:%S] >>> %v <<<"));

      std::cout << "\nPattern 2: [%H:%M:%S] >>> %v <<<\n"; sink->log(msg);

      // 再次修改
      sink->set_formatter(std::make_unique<pattern_formatter>("%Y/%m/%d | %v"));

      std::cout << "\nPattern 3: %Y/%m/%d | %v\n"; sink->log(msg););
}

// NOLINTNEXTLINE
TEST_CASE("test_thread_id") {
  std::cout << "\n========== 测试9:多线程 ID 显示 ==========\n";
  CHECK_NOTHROW(
      auto sink = std::make_shared<sinks::console_sink_mt>();
      sink->set_formatter(
          std::make_unique<pattern_formatter>("[thread %t] %v"));
      auto log_from_thread =
          [&](int thread_num) {
            std::string s = "Message From thread" + std::to_string(thread_num);
            details::log_message msg("ThreadTest", level::info, s);
            sink->log(msg);
          };

      std::thread t1(log_from_thread, 1); std::thread t2(log_from_thread, 2);
      std::thread t3(log_from_thread, 3);

      t1.join(); t2.join(); t3.join(););
}

// NOLINTNEXTLINE
TEST_CASE("test_unknown_flags") {
  std::cout << "\n========== 测试10:未知占位符处理 ==========\n";
  CHECK_NOTHROW(
      pattern_formatter formatter("[%Y-%m-%d] [%Z] %v"); // %Z 是未知占位符
      details::log_message msg("TestLogger", level::info, "Test unknown flags");
      fmt::memory_buffer buf; formatter.format(msg, buf);

      std::cout << "Pattern: [%Y-%m-%d] [%Z] %v\n";
      std::cout << "Output:  " << std::string(buf.data(), buf.size());
      std::cout << "说明: 未知占位符 %Z 被原样输出\n";);
}