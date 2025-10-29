#pragma once

#include "mispdlog/common.h"
#include "mispdlog/details/log_message.h"
#include <memory>

namespace mispdlog {
class logger;
namespace details {
enum class async_message_type { log, flush, terminate };

struct log_message_buffer : log_message {
  std::string buffer;

  log_message_buffer() = default;

  explicit log_message_buffer(const log_message &message)
      : log_message(message),
        buffer(message.payload.data(), message.payload.size()) {
    payload = string_view_t(buffer);
  }

  log_message_buffer(log_message_buffer &&other)
      : log_message(other), buffer(std::move(other.buffer)) {
    payload = string_view_t(buffer.data(), buffer.size());
  }

  log_message_buffer &operator=(log_message_buffer &&other) {
    if (this != &other) {
      log_message::operator=(other);
      buffer = std::move(other.buffer);
      payload = string_view_t(buffer.data(), buffer.size());
    }
    return *this;
  }
};

struct async_message : log_message_buffer {
  async_message_type type{async_message_type::log};

  std::shared_ptr<logger> log_ptr;

  async_message() = default;
  ~async_message() = default;

  explicit async_message(async_message_type type)
      : async_message(type, nullptr) {}

  /**
   * @brief Construct a new async message object,强制转移所有权,worker  这个
   * logger  对象只应该被消费一次。 如果允许左值传进来，调用者仍保留一份
   * shared_ptr ，后续可能误用，造成多线程访问同一个 logger 的风险。
   *
   * @param type
   * @param ptr
   * @param message
   */
  async_message(async_message_type type, std::shared_ptr<logger> &&ptr,
                const log_message &message)
      : log_message_buffer(message), type(type), log_ptr(std::move(ptr)) {}

  async_message(async_message_type type, std::shared_ptr<logger> &&ptr)
      : log_message_buffer{}, type(type), log_ptr(std::move(ptr)) {}

  async_message(const async_message &) = delete;
  async_message &operator=(const async_message &) = delete;

  async_message(async_message &&other)
      : log_message_buffer(std::move(other)), type(other.type),
        log_ptr(other.log_ptr) {}

  async_message &operator=(async_message &&other) {
    if (this != &other) {
      log_message_buffer::operator=(std::move(other));
      type = other.type;
      log_ptr = std::move(other.log_ptr);
    }
    return *this;
  }
};

} // namespace details
} // namespace mispdlog