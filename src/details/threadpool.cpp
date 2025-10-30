#include "mispdlog/details/threadpool.h"
#include "mispdlog/details/async_message.h"
#include <chrono>
#include <cstddef>
#include <stdexcept>
#include <utility>

namespace mispdlog {
namespace details {

threadpool::threadpool(size_t queue_size, size_t threads_n)
    : queue_(queue_size) {
  if (threads_n == 0 || threads_n > 1000) {
    throw std::invalid_argument("thread_pool: threads_n must be 1-1000");
  }

  for (size_t i = 0; i < threads_n; i++) {
    threads_.emplace_back([this] { this->worker_loop_(); });
  }
}

threadpool::~threadpool() {
  for (size_t i = 0; i < threads_.size(); i++) {
    async_message terminate_message;
    terminate_message.type = async_message_type::terminate;
    queue_.enqueue(std::move(terminate_message));
  }

  for (auto &thread : threads_) {
    if (thread.joinable()) {
      thread.join();
    }
  }
}

void threadpool::post_log(std::shared_ptr<async_logger> &&logger_ptr,
                          const log_message &message) {
  async_message async_message(async_message_type::log, std::move(logger_ptr),
                              message);
  queue_.enqueue(std::move(async_message));
}

void threadpool::post_log_nowait(std::shared_ptr<async_logger> &&logger_ptr,
                                 const log_message &message) {
  async_message async_message(async_message_type::log, std::move(logger_ptr),
                              message);
  queue_.enqueue_nowait(std::move(async_message));
}

void threadpool::post_flush(std::shared_ptr<async_logger> &&logger_ptr) {
  async_message async_message(async_message_type::flush, std::move(logger_ptr));
  queue_.enqueue(std::move(async_message));
}

size_t threadpool::overrun_counter() { return queue_.overrun_counter(); }

void threadpool::worker_loop_() {
  while (process_next_message_()) {
  }
}

bool threadpool::process_next_message_() {
  async_message message;
  if (!queue_.dequeue_for(message, std::chrono::seconds(10))) {
    return true;
  }
  switch (message.type) {
  case async_message_type::log: {
    if (message.log_ptr) {
      message.log_ptr->backend_sink_it_(message);
    }
    return true;
  }
  case async_message_type::flush: {
    if (message.log_ptr) {
      message.log_ptr->backend_flush_();
    }
    return true;
  }
  case async_message_type::terminate: {
    return false;
  }
  }
  return true;
}

} // namespace details
} // namespace mispdlog