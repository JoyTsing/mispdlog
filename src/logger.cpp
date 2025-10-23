#include "mispdlog/logger.h"
#include "mispdlog/sinks/base_sink.h"
#include <algorithm>

namespace mispdlog {
logger::logger(std::string name) : name_(std::move(name)) {}

logger::logger(std::string name, sinks::sink_ptr single_sink)
    : name_(std::move(name)) {
  sinks_.emplace_back(single_sink);
}

logger::logger(std::string name, std::vector<sinks::sink_ptr> multi_sink)
    : name_(std::move(name)), sinks_(multi_sink) {}

void logger::add_sink(sinks::sink_ptr sink) { sinks_.emplace_back(sink); }

void logger::remove_sink(sinks::sink_ptr sink) {
  sinks_.erase(std::remove(sinks_.begin(), sinks_.end(), sink), sinks_.end());
}

std::vector<sinks::sink_ptr> &logger::sinks() { return sinks_; }

const std::vector<sinks::sink_ptr> &logger::sinks() const { return sinks_; }

void logger::set_level(level level) { level_ = level; }

level logger::get_level() const { return level_; }

bool logger::should_log(level message_level) const {
  return message_level >= level_;
}

void logger::flush() {
  for (const auto &sink : sinks_) {
    sink->flush();
  }
}

void logger::flush_when(level level) { flush_level_ = level; }

const std::string &logger::name() const { return name_; }

void logger::sink_it_(const details::log_message &message) {
  for (const auto &sink : sinks_) {
    if (sink->should_log(message.level)) {
      sink->log(message);
    }
  }
  if (message.level >= flush_level_) {
    flush();
  }
}

} // namespace mispdlog