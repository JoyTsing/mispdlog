#include <sys/stat.h>
#define ANKERL_NANOBENCH_IMPLEMENT
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "mispdlog/async.h"
#include "mispdlog/mispdlog.h"

#include <doctest.h>
#include <fcntl.h>
#include <nanobench.h>

bool create_directory(const std::string &path) {
#ifdef _WIN32
  return _mkdir(path.c_str()) == 0 || errno == EEXIST;
#else
  return mkdir(path.c_str(), 0755) == 0 || errno == EEXIST;
#endif
}

// NOLINTNEXTLINE
TEST_CASE("test_basic_async") {
  CHECK_NOTHROW(
      std::cout << "\n========== 测试1:基础异步日志 ==========" << std::endl;

      // 创建异步彩色控制台 logger
      auto logger = mispdlog::async_stdout_color_mt("async_console");

      // 设置日志级别为 trace,确保所有消息都能输出
      logger->set_level(mispdlog::level::trace);

      // 输出不同级别的日志
      logger->trace("This is trace message");
      logger->debug("This is debug message");
      logger->info("Hello from async logger!");
      logger->warn("This is a warning"); logger->error("This is an error");
      logger->critical("This is critical!");

      // 重要:显式刷新,确保消息被处理
      logger->flush();

      // 等待后台线程处理完消息
      std::this_thread::sleep_for(std::chrono::milliseconds(100));

      std::cout << "✓ 基础异步日志测试通过" << std::endl;);
}

// NOLINTNEXTLINE
TEST_CASE("test_async_file") {
  CHECK_NOTHROW(
      std::cout << "\n========== 测试2:异步文件日志 ==========" << std::endl;

      // 确保 logs 目录存在
      if (!create_directory("logs")) {
        std::cerr << "警告: 无法创建 logs 目录,但可能已存在" << std::endl;
      }

      // 创建异步文件 logger
      auto logger =
          mispdlog::async_file_mt("async_file", "logs/async_test.log", true);

      // 输出大量日志
      for (int i = 0; i < 100;
           ++i) { logger->info("Async log message #{}", i); }

      // 显式刷新
      logger->flush();

      // 等待刷新完成
      std::this_thread::sleep_for(std::chrono::milliseconds(200));

      std::cout << "✓ 异步文件日志测试通过 (logs/async_test.log)"
                << std::endl;);
}

// NOLINTNEXTLINE
TEST_CASE("test_overflow_policy") {
  CHECK_NOTHROW(
      std::cout << "\n========== 测试3:溢出策略 ==========" << std::endl;

      // 初始化小队列(容易满)
      mispdlog::init_thread_pool(10, 1);

      // 先删除旧的 logger
      mispdlog::drop("async_block"); mispdlog::drop("async_overrun");

      // block 策略:队列满时阻塞
      auto logger_block = mispdlog::async_stdout_color_mt(
          "async_block", mispdlog::async_overflow_policy::block);

      std::cout << "测试 block 策略(队列满时阻塞)..." << std::endl;
      for (int i = 0; i < 50;
           ++i) { logger_block->info("Block policy message #{}", i); }

      logger_block->flush();
      std::this_thread::sleep_for(std::chrono::milliseconds(300));

      // 删除旧 logger,重新初始化线程池
      mispdlog::drop("async_block");

      // overrun_oldest 策略:队列满时覆盖最旧消息
      mispdlog::init_thread_pool(10, 1); // 重新初始化

      auto logger_overrun = mispdlog::async_stdout_color_mt(
          "async_overrun", mispdlog::async_overflow_policy::overrun_oldest);

      std::cout << "\n测试 overrun_oldest 策略(队列满时覆盖)..." << std::endl;
      for (int i = 0; i < 50;
           ++i) { logger_overrun->warn("Overrun policy message #{}", i); }

      logger_overrun->flush();
      std::this_thread::sleep_for(std::chrono::milliseconds(300));

      std::cout << "✓ 溢出策略测试通过" << std::endl;);
}

// NOLINTNEXTLINE
TEST_CASE("test_multi_thread_logging") {
  CHECK_NOTHROW(
      std::cout << "\n========== 测试4:多线程异步日志 ==========" << std::endl;

      // 删除旧 logger
      mispdlog::drop("multi_thread_async");

      // 重新初始化合理大小的队列
      mispdlog::init_thread_pool(8192, 2); // 2个工作线程

      auto logger = mispdlog::async_stdout_color_mt("multi_thread_async");

      // 创建多个线程同时写日志
      std::vector<std::thread> threads; constexpr int thread_count = 5;
      constexpr int messages_per_thread = 20;

      for (int t = 0; t < thread_count; ++t) {
        threads.emplace_back([logger, t]() {
          for (int i = 0; i < messages_per_thread; ++i) {
            logger->info("Thread {} - Message {}", t, i);
          }
        });
      }

      // 等待所有线程完成
      for (auto &thread
           : threads) { thread.join(); }

      // 显式刷新
      logger->flush();

      // 等待后台处理完成
      std::this_thread::sleep_for(std::chrono::milliseconds(500));

      std::cout << "✓ 多线程异步日志测试通过" << std::endl;);
}

// NOLINTNEXTLINE
TEST_CASE("test_async_rotating_file") {
  CHECK_NOTHROW(
      std::cout << "\n========== 测试5:异步滚动文件 ==========" << std::endl;

      // 确保 logs 目录存在
      create_directory("logs");

      // 删除旧 logger
      mispdlog::drop("async_rotating");

      mispdlog::init_thread_pool(8192, 1);

      // 创建异步滚动文件 logger(每个文件最大 1KB,最多保留 3 个)
      auto logger = mispdlog::async_rotating_logger_mt(
          "async_rotating", "logs/async_rotating.log",
          1024, // 1KB
          3);

      // 输出大量日志触发文件滚动
      for (int i = 0; i < 200; ++i) {
        logger->info(
            "Rotating log message #{} - some padding text to make it longer",
            i);
      }

      logger->flush();
      std::this_thread::sleep_for(std::chrono::milliseconds(500));

      std::cout << "✓ 异步滚动文件测试通过 (logs/async_rotating.log)"
                << std::endl;);
}
