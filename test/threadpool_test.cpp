#define ANKERL_NANOBENCH_IMPLEMENT
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "mispdlog/details/circular_queue.h"
#include "mispdlog/details/mpmc_blocking_queue.h"
#include "mispdlog/details/threadpool.h"
#include "mispdlog/sinks/console_sink.h"

#include <doctest.h>
#include <nanobench.h>
#include <thread>

using namespace mispdlog;

// NOLINTNEXTLINE
TEST_CASE("test_circular_queue") {
  std::cout << "\n========== 测试1:循环队列基础功能 ==========\n";
  CHECK_NOTHROW(
      details::circular_queue<int> q(5); // 容量为5

      std::cout << "容量: " << q.capacity() << "\n";
      std::cout << "初始大小: " << q.size() << "\n";
      std::cout << "是否为空: " << (q.empty() ? "是" : "否") << "\n\n";

      // 添加元素
      std::cout << "添加元素 1, 2, 3:\n"; q.push_back(1); q.push_back(2);
      q.push_back(3);

      std::cout << "当前大小: " << q.size() << "\n";
      std::cout << "队首元素: " << q.front() << "\n\n";

      // 移除元素
      std::cout << "移除队首:\n"; q.pop_front();
      std::cout << "当前大小: " << q.size() << "\n";
      std::cout << "新队首: " << q.front() << "\n\n";

      // 测试满队列
      std::cout << "继续添加直到满:\n"; q.push_back(4); q.push_back(5);
      q.push_back(6);

      std::cout << "是否已满: " << (q.full() ? "是" : "否") << "\n";
      std::cout << "当前大小: " << q.size() << "\n\n";

      // 测试溢出(覆盖)
      std::cout << "再添加元素(会覆盖最旧的):\n"; q.push_back(7);

      std::cout << "溢出次数: " << q.overrun_counter() << "\n";
      std::cout << "队首元素: " << q.front() << " (应该是3,因为2被覆盖了)\n";);
}

// NOLINTNEXTLINE
TEST_CASE("test_mpmc_queue_basic") {
  std::cout << "\n========== 测试2:MPMC队列基础功能 ==========\n";
  CHECK_NOTHROW(
      details::mpmc_blocking_queue<int> q(10);

      // 入队
      std::cout << "入队 3 个元素...\n"; q.enqueue(100); q.enqueue(200);
      q.enqueue(300);

      std::cout << "队列大小: " << q.size() << "\n\n";

      // 出队
      std::cout << "出队:\n"; int value;
      if (q.dequeue_for(value, std::chrono::milliseconds(100))) {
        std::cout << "  出队值: " << value << "\n";
      }

      if (q.dequeue_for(value, std::chrono::milliseconds(100))) {
        std::cout << "  出队值: " << value << "\n";
      }

      std::cout
      << "剩余大小: " << q.size() << "\n";);
}

// NOLINTNEXTLINE
TEST_CASE("test_mpmc_queue_timeout") {
  std::cout << "\n========== 测试3:MPMC队列超时机制 ==========\n";
  CHECK_NOTHROW(
      details::mpmc_blocking_queue<int> q(5);

      std::cout << "尝试从空队列出队(100ms超时)...\n";

      int value; auto start = std::chrono::high_resolution_clock::now();
      bool success = q.dequeue_for(value, std::chrono::milliseconds(100));
      auto end = std::chrono::high_resolution_clock::now();

      auto duration =
          std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

      std::cout << "结果: " << (success ? "成功" : "超时") << "\n";
      std::cout << "耗时: " << duration.count() << " ms\n";

      if (!success) { std::cout << " 超时机制正常工作\n"; });
}

// NOLINTNEXTLINE
TEST_CASE("test_mpmc_queue_blocking") {
  CHECK_NOTHROW(
      std::cout << "\n========== 测试4:MPMC队列阻塞机制 ==========\n";);
  details::mpmc_blocking_queue<int> q(3); // 小队列

  std::atomic<bool> producer_started{false};

  // 生产者线程:先填满队列,然后阻塞
  std::thread producer([&]() {
    std::cout << "生产者:填满队列...\n";
    q.enqueue(1);
    q.enqueue(2);
    q.enqueue(3);

    producer_started = true;

    std::cout << "生产者:队列已满,下一次入队将阻塞...\n";
    q.enqueue(4); // 这里会阻塞
    std::cout << "生产者:入队成功(消费者已取走元素)\n";
  });

  // 等待生产者填满队列
  while (!producer_started) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // 消费者:取走元素,解除生产者阻塞
  std::cout << "消费者:取出一个元素...\n";
  int value;
  q.dequeue_for(value, std::chrono::seconds(1));
  std::cout << "消费者:取出值 " << value << "\n";

  producer.join();
  std::cout << " 阻塞/唤醒机制正常\n";
}

// NOLINTNEXTLINE
TEST_CASE("test_mpmc_queue_concurrent") {
  CHECK_NOTHROW(
      std::cout << "\n========== 测试5:MPMC队列并发性能 ==========\n";

      details::mpmc_blocking_queue<int> q(1000);

      const int num_producers = 4; const int num_consumers = 2;
      const int items_per_producer = 1000000;

      std::atomic<int> total_produced{0}; std::atomic<int> total_consumed{0};

      auto start = std::chrono::high_resolution_clock::now();

      // 启动生产者
      std::vector<std::thread> producers;
      for (int i = 0; i < num_producers; ++i) {
        producers.emplace_back([&, i]() {
          for (int j = 0; j < items_per_producer; ++j) {
            q.enqueue(i * items_per_producer + j);
            total_produced++;
          }
        });
      }

      // 启动消费者
      std::vector<std::thread>
          consumers;
      for (int i = 0; i < num_consumers; ++i) {
        consumers.emplace_back([&]() {
          int value;
          while (total_consumed < num_producers * items_per_producer) {
            if (q.dequeue_for(value, std::chrono::milliseconds(100))) {
              total_consumed++;
            }
          }
        });
      }

      // 等待所有线程完成
      for (auto &t
           : producers) t.join();
      for (auto &t
           : consumers) t.join();

      auto end = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

      std::cout << "生产者数量: " << num_producers << "\n";
      std::cout << "消费者数量: " << num_consumers << "\n";
      std::cout << "总消息数: " << num_producers * items_per_producer << "\n";
      std::cout << "生产总数: " << total_produced << "\n";
      std::cout << "消费总数: " << total_consumed << "\n";
      std::cout << "耗时: " << duration.count() << " ms\n";
      std::cout << "吞吐量: " << (total_consumed * 1000.0 / duration.count())
                << " 条/秒\n";

      if (total_produced == total_consumed) { std::cout << " 无消息丢失\n"; });
}

// NOLINTNEXTLINE
TEST_CASE("test_thread_pool_basic") {
  CHECK_NOTHROW(
      std::cout << "\n========== 测试6:线程池基础功能 ==========\n";

      // 创建线程池: 队列大小1024, 2个工作线程
      details::threadpool pool(1024, 2);

      // 创建一个简单的 logger
      auto console_sink = std::make_shared<sinks::console_sink_mt>();

      std::cout << "向线程池投递3条日志消息:\n\n";

      // 投递日志消息 - 为每次调用创建新的 shared_ptr
      details::log_message msg1("test", level::info,
                                "Message 1 from thread pool");
      auto test_logger1 = std::make_shared<logger>("test", console_sink);
      pool.post_log(std::move(test_logger1), msg1);

      details::log_message msg2("test", level::warn,
                                "Message 2 from thread pool");
      auto test_logger2 = std::make_shared<logger>("test", console_sink);
      pool.post_log(std::move(test_logger2), msg2);

      details::log_message msg3("test", level::error,
                                "Message 3 from thread pool");
      auto test_logger3 = std::make_shared<logger>("test", console_sink);
      pool.post_log(std::move(test_logger3), msg3);

      // 等待处理
      std::this_thread::sleep_for(std::chrono::milliseconds(500));

      std::cout << "\n 线程池处理完成\n";);
}

// NOLINTNEXTLINE
TEST_CASE("test_thread_pool_performance") {
  CHECK_NOTHROW(
      std::cout << "\n========== 测试7:线程池性能 ==========\n";

      details::threadpool pool(8192, 2);

      auto console_sink = std::make_shared<sinks::console_sink_mt>();
      console_sink->set_level(level::off); // 关闭输出,只测试队列性能

      const int num_messages = 100000;

      std::cout << "投递 " << num_messages << " 条消息到线程池...\n";

      auto start = std::chrono::high_resolution_clock::now();

      for (int i = 0; i < num_messages; ++i) {
        details::log_message msg("perf", level::info,
                                 "Performance test message");
        auto test_logger = std::make_shared<logger>("perf", console_sink);
        pool.post_log(std::move(test_logger), msg);
      }

      auto end = std::chrono::high_resolution_clock::now();
      auto enqueue_duration =
          std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

      std::cout << "入队耗时: " << enqueue_duration.count() << " ms\n";
      std::cout << "入队吞吐: "
                << (num_messages * 1000.0 / enqueue_duration.count())
                << " 条/秒\n";

      // 等待处理完成
      std::this_thread::sleep_for(std::chrono::seconds(2));

      std::cout << "溢出次数: " << pool.overrun_counter() << "\n";);
}

// NOLINTNEXTLINE
TEST_CASE("test_thread_pool_multithreaded") {
  CHECK_NOTHROW(
      std::cout << "\n========== 测试8:多线程使用线程池 ==========\n";

      details::threadpool pool(4096, 3);

      auto console_sink = std::make_shared<sinks::console_sink_mt>();
      console_sink->set_level(level::off);
      auto test_logger = std::make_shared<logger>("mt", console_sink);

      const int num_threads = 8; const int messages_per_thread = 100000;

      std::atomic<int> total_sent{0};

      auto start = std::chrono::high_resolution_clock::now();

      std::vector<std::thread> threads;
      for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&]() {
          for (int j = 0; j < messages_per_thread; ++j) {
            details::log_message msg("mt", level::info, "Test message");
            pool.post_log(std::move(test_logger), msg);
            total_sent++;
          }
        });
      }

      for (auto &t
           : threads) { t.join(); }

      auto end = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

      std::cout << "线程数: " << num_threads << "\n";
      std::cout << "总消息数: " << total_sent << "\n";
      std::cout << "耗时: " << duration.count() << " ms\n";
      std::cout << "吞吐量: " << (total_sent * 1000.0 / duration.count())
                << " 条/秒\n";

      // 等待处理
      std::this_thread::sleep_for(std::chrono::seconds(1));

      std::cout << " 多线程并发测试完成\n";);
}

// NOLINTNEXTLINE
TEST_CASE("test_overflow_policy") {
  CHECK_NOTHROW(
      std::cout << "\n========== 测试9:溢出策略 ==========\n";

      // 创建小队列测试溢出
      details::threadpool pool(100, 1);

      auto console_sink = std::make_shared<sinks::console_sink_mt>();
      console_sink->set_level(level::off);
      auto test_logger = std::make_shared<logger>("overflow", console_sink);

      std::cout << "快速投递大量消息到小队列(容量100)...\n";

      // 使用 nowait 模式快速填满队列
      for (int i = 0; i < 10000; ++i) {
        details::log_message msg("overflow", level::info, "Overflow test");
        pool.post_log_nowait(std::move(test_logger), msg);
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(100));

      size_t overruns = pool.overrun_counter();
      std::cout << "溢出次数: " << overruns << "\n";

      if (overruns > 0) { std::cout << " 溢出策略正常工作(覆盖旧消息)\n"; });
}

// NOLINTNEXTLINE
TEST_CASE("test_graceful_shutdown") {
  CHECK_NOTHROW(
      std::cout << "\n========== 测试10:优雅关闭 ==========\n";

      std::cout << "创建线程池并投递消息...\n";

      {
        details::threadpool pool(1024, 2);

        auto console_sink = std::make_shared<sinks::console_sink_mt>();
        console_sink->set_level(level::off);

        for (int i = 0; i < 100; ++i) {
          details::log_message msg("shutdown", level::info, "Shutdown test");
          auto test_logger = std::make_shared<logger>("shutdown", console_sink);
          pool.post_log(std::move(test_logger), msg);
        }

        std::cout << "线程池即将销毁...\n";
      }
      // 线程池析构,应该等待所有线程正常结束

      std::cout
      << " 线程池已优雅关闭\n";);
}