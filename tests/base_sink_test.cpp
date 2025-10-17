#define ANKERL_NANOBENCH_IMPLEMENT
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "mispdlog/details/utils.h"
#include "mispdlog/level.h"
#include "mispdlog/sinks/console_sink.h"

#include <doctest.h>
#include <fcntl.h>
#include <memory>
#include <nanobench.h>

using namespace mispdlog;

// NOLINTNEXTLINE
TEST_CASE("test logoutput") {
  std::cout << "\n========== 测试1:log_msg 创建 ==========\n";
  CHECK_NOTHROW(
      // 创建日志消息
      details::log_message msg1("TestLogger", level::info,
                                "Hello, MiniSpdlog!");

      std::cout << "Logger 名称: " << msg1.logger_name << "\n";
      std::cout << "日志级别: " << level_to_string(msg1.level) << "\n";
      std::cout << "线程 ID: " << msg1.thread_id << "\n";
      std::cout << "消息内容: " << msg1.payload << "\n";
      std::cout << "源码位置为空: " << (msg1.loc.empty() ? "是" : "否")
                << "\n";);
}
// NOLINTNEXTLINE
TEST_CASE("test source location simple") {
  std::cout << "\n========== 测试2:源码位置信息 ==========\n";
  CHECK_NOTHROW(
      details::source_location loc("test_sink.cpp", 42, "test_function");
      details::log_message msg("TestLogger", level::debug, loc,
                               "Debug message with source info");

      std::cout << "文件名: "
                << (msg.loc.filename.empty() ? "null" : msg.loc.filename)
                << "\n";
      std::cout << "行号: " << msg.loc.line << "\n";
      std::cout << "函数名: "
                << (msg.loc.function_name.empty() ? "null"
                                                  : msg.loc.function_name)
                << "\n";);
}
// NOLINTNEXTLINE
TEST_CASE("test multi-thread console sink") {
  std::cout << "\n========== 测试3:多线程控制台 Sink ==========\n";
  CHECK_NOTHROW(auto sink = std::make_shared<sinks::console_sink_mt>();
                sink->set_level(level::trace);
                // 测试不同级别的日志
                std::vector<level> levels = {level::trace, level::debug,
                                             level::info, level::warn,
                                             level::error, level::critical};
                for (auto lvl
                     : levels) {
                  std::string msg_content =
                      std::string("This is a ") +
                      details::to_string(level_to_string(lvl)) + " message";
                  details::log_message msg("TestLogger", lvl, msg_content);

                  if (sink->should_log(lvl)) {
                    sink->log(msg);
                  }
                }

                sink->flush(););
}
// NOLINTNEXTLINE
TEST_CASE("test logoutput filtering") {
  std::cout << "\n========== 测试4:日志级别过滤 ==========\n";
  CHECK_NOTHROW(
      auto sink = std::make_shared<sinks::console_sink_mt>();
      sink->set_level(level::warn);
      std::cout << "Sink 级别设置为: " << level_to_string(sink->get_level())
                << "\n\n";
      std::vector<level> test_levels = {level::trace, level::debug, level::info,
                                        level::warn, level::error,
                                        level::critical};

      for (auto lvl
           : test_levels) {
        std::string msg_content =
            std::string("Testing ") + details::to_string(level_to_string(lvl));
        details::log_message msg("FilterTest", lvl, msg_content);

        bool should = sink->should_log(lvl);
        std::cout << level_to_string(lvl) << " - "
                  << (should ? "✓ 会输出" : "✗ 被过滤") << "\n";

        sink->log(msg);
      });
}
// NOLINTNEXTLINE
TEST_CASE("test stderr logoutput") {
  std::cout << "\n========== 测试5:stderr Sink ==========\n";
  CHECK_NOTHROW(auto err_sink = std::make_shared<sinks::stderr_sink_mt>();
                err_sink->set_level(level::error);

                std::cout << "(以下消息应该输出到 stderr)\n";

                details::log_message error_msg("ErrorLogger", level::error,
                                               "This is an error message");
                details::log_message critical_msg("ErrorLogger",
                                                  level::critical,
                                                  "This is a critical message");

                err_sink->log(error_msg); err_sink->log(critical_msg);
                err_sink->flush(););
}
// NOLINTNEXTLINE
TEST_CASE("test logoutput") {
  auto mute_io = [&]() {
#ifdef _WIN32
    fflush(stderr);
    freopen("NUL", "w", stderr);
#else
    fflush(stderr);
    freopen("/dev/null", "w", stderr);
#endif
  };

  std::cout << "\n========== 测试6:性能对比提示 ==========\n";
  CHECK_NOTHROW(
      auto sink_mt = std::make_shared<sinks::stderr_sink_mt>();
      auto sink_st = std::make_shared<sinks::stderr_sink_st>();
      ankerl::nanobench::Bench bench; mute_io(); details::log_message msg(
          "Logger", level::info, "Test message, it should be long enough");
      bench.minEpochIterations(100000).run("MTLogger",
                                           [&]() { sink_mt->log(msg); });
      bench.minEpochIterations(100000).run("STLogger",
                                           [&]() { sink_st->log(msg); });

  );
}
