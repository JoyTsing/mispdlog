#pragma once

#include "mispdlog/async_logger.h"
#include "mispdlog/common.h"
#include "mispdlog/details/async_message.h"
#include "mispdlog/details/log_message.h"
#include "mispdlog/details/mpmc_blocking_queue.h"

#include <cstddef>
#include <memory>
#include <thread>
#include <vector>

namespace mispdlog {
namespace details {
struct async_message;
class MISPDLOG_API threadpool {
public:
  /**
   * @brief Construct a new threadpool object
   *
   * @param queue_size
   * @param threads_n
   */
  threadpool(size_t queue_size, size_t threads_n);
  ~threadpool();

  threadpool(const threadpool &) = delete;
  threadpool &operator=(const threadpool &) = delete;

  void post_log(std::shared_ptr<async_logger> &&logger_ptr,
                const log_message &message);

  void post_log_nowait(std::shared_ptr<async_logger> &&logger_ptr,
                       const log_message &message);

  void post_flush(std::shared_ptr<async_logger> &&logger_ptr);

  size_t overrun_counter();

private:
  void worker_loop_();
  bool process_next_message_();

  mpmc_blocking_queue<async_message> queue_;
  std::vector<std::thread> threads_;
};

} // namespace details
} // namespace mispdlog