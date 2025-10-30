#include "mispdlog/async_logger.h"
#include <stdexcept>

namespace mispdlog {
async_logger::async_logger(std::string name, sinks::sink_ptr single_sink,
                           std::weak_ptr<details::threadpool> tp,
                           async_overflow_policy policy)
    : logger(std::move(name), std::move(single_sink)),
      threadpool_(std::move(tp)), overflow_policy_(policy) {}

async_logger::async_logger(std::string name, std::vector<sinks::sink_ptr> sinks,
                           std::weak_ptr<details::threadpool> tp,
                           async_overflow_policy policy)
    : logger(std::move(name), std::move(sinks)), threadpool_(std::move(tp)),
      overflow_policy_(policy) {}

void async_logger::sink_it_(const details::log_message &msg) {
  if (auto pool_ptr = threadpool_.lock()) {
    if (overflow_policy_ == async_overflow_policy::block) {
      pool_ptr->post_log(shared_from_this(), msg);
    } else if (overflow_policy_ == async_overflow_policy::overrun_oldest) {
      pool_ptr->post_log_nowait(shared_from_this(), msg);
    }
  } else {
    throw std::runtime_error(
        "async_logger::sink_it_: threadpool doesn't exist anymore");
  }
}

void async_logger::flush_() {
  if (auto pool_ptr = threadpool_.lock()) {
    pool_ptr->post_flush(shared_from_this());
  } else {
    throw std::runtime_error(
        "async_logger::flush_: threadpool doesn't exist anymore");
  }
}

void async_logger::backend_sink_it_(const details::log_message &msg) {
  for (auto &sink : sinks_) {
    if (sink->should_log(msg.level)) {
      sink->log(msg);
    }
  }
  if (msg.level >= flush_level_) {
    backend_flush_();
  }
}

void async_logger::backend_flush_() {
  for (auto &sink : sinks_) {
    sink->flush();
  }
}
} // namespace mispdlog