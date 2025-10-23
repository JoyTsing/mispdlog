#include "mispdlog/registry.h"
#include "mispdlog/level.h"
#include "mispdlog/logger.h"
#include "mispdlog/sinks/color_console_sink.h"
#include <memory>
#include <mutex>
#include <stdexcept>

namespace mispdlog {
registry::registry() { recover_default_(); }

registry &registry::instance() {
  static registry instance;
  return instance;
}

void registry::register_logger(std::shared_ptr<logger> logger) {
  std::lock_guard<std::mutex> lock(mutex_);
  throw_if_exists_(logger->name());
  mp_[logger->name()] = logger;
}

std::shared_ptr<logger> registry::get(const std::string &logger_name) {
  std::lock_guard<std::mutex> lock(mutex_);
  auto it = mp_.find(logger_name);
  if (it != mp_.end()) {
    return it->second;
  }
  return nullptr;
}

void registry::drop(const std::string &logger_name) {
  std::lock_guard<std::mutex> lock(mutex_);
  auto is_default = default_logger_ && default_logger_->name() == logger_name;
  mp_.erase(logger_name);
  if (is_default) {
    default_logger_.reset();
    recover_default_();
  }
}

void registry::drop_all() {
  std::lock_guard<std::mutex> lock(mutex_);
  mp_.clear();
  default_logger_.reset();
  recover_default_();
}

std::shared_ptr<logger> registry::default_logger() {
  std::lock_guard<std::mutex> lock(mutex_);
  return default_logger_;
}

void registry::set_default_logger(std::shared_ptr<logger> new_default_logger) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (new_default_logger != nullptr) {
    mp_[new_default_logger->name()] = new_default_logger;
  }
  default_logger_ = new_default_logger;
}

void registry::set_all_level(level level) {
  std::lock_guard<std::mutex> lock(mutex_);
  default_logger_->set_level(level);
  for (auto [_, logger] : mp_) {
    logger->set_level(level);
  }
}

void registry::recover_default_() {
  auto sink = std::make_shared<sinks::color_console_sink_mt>();
  default_logger_ = std::make_shared<logger>("", sink);
  default_logger_->set_level(level::info);
}

void registry::flush_all_loggers() {
  std::lock_guard<std::mutex> lock(mutex_);
  default_logger_->flush();
  for (auto [_, logger] : mp_) {
    logger->flush();
  }
}

void registry::throw_if_exists_(const std::string &logger_name) {
  if (mp_.find(logger_name) != mp_.end()) {
    throw std::runtime_error("logger with name '" + logger_name +
                             "' already exists.");
  }
}
} // namespace mispdlog