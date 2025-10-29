#pragma once

#include <cstddef>
#include <utility>
#include <vector>
namespace mispdlog {
namespace details {
template <typename T> class circular_queue {
public:
  explicit circular_queue(size_t max_items)
      : v_(max_items + 1), max_items_(max_items + 1), head_(0), tail_(0),
        overrun_counter_(0) {}

  circular_queue(const circular_queue &) = delete;
  circular_queue &operator=(const circular_queue &) = delete;

  void push_back(T &&item) {
    v_[tail_] = std::move(std::forward<T>(item));
    tail_ = (tail_ + 1) % max_items_;

    if (tail_ == head_) {
      head_ = (head_ + 1) % max_items_;
      overrun_counter_++;
    }
  }

  void clear() { head_ = tail_ = 0; }

  const T &front() const { return v_[head_]; }

  T &front() { return v_[head_]; }

  void pop_front() { head_ = (head_ + 1) % max_items_; }

  bool full() const { return ((tail_ + 1) % max_items_) == head_; }

  bool empty() const { return tail_ == head_; }

  size_t size() const {
    if (tail_ >= head_) {
      return tail_ - head_;
    }
    return max_items_ - (head_ - tail_);
  }

  size_t capacity() const { return max_items_ - 1; }

  size_t overrun_counter() const { return overrun_counter_; }

private:
  std::vector<T> v_;

  size_t max_items_;
  size_t head_;
  size_t tail_;
  size_t overrun_counter_;
};

} // namespace details
} // namespace mispdlog