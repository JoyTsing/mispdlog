#pragma once

#include "mispdlog/details/circular_queue.h"
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <utility>

namespace mispdlog {
namespace details {
template <typename T> class mpmc_blocking_queue {
public:
  explicit mpmc_blocking_queue(size_t max_items) : q_(max_items) {}

  mpmc_blocking_queue(const mpmc_blocking_queue &) = delete;
  mpmc_blocking_queue &operator=(const mpmc_blocking_queue &) = delete;

  void enqueue(T &&item) {

    std::unique_lock<std::mutex> lock(mutex_);
    pop_cv_.wait(lock, [this] { return !this->q_.full(); });
    q_.push_back(std::move(item));

    push_cv_.notify_one();
  }

  void enqueue_nowait(T &&item) {

    std::unique_lock<std::mutex> lock(mutex_);
    q_.push_back(std::move(item));

    push_cv_.notify_one();
  }

  bool dequeue_for(T &pop_item,
                   std::chrono::milliseconds wait = std::chrono::milliseconds{
                       1000}) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (!push_cv_.wait_for(lock, wait, [this] { return !this->q_.empty(); })) {
      return false;
    }
    pop_item = std::move(q_.front());
    q_.pop_front();
    pop_cv_.notify_one();
    return true;
  }

  size_t overrun_counter() {
    std::lock_guard<std::mutex> lock(mutex_);
    return q_.overrun_counter();
  }

  size_t size() {
    std::lock_guard<std::mutex> lock(mutex_);
    return q_.size();
  }

private:
  std::mutex mutex_;
  std::condition_variable push_cv_;
  std::condition_variable pop_cv_;
  circular_queue<T> q_;
};
} // namespace details
} // namespace mispdlog