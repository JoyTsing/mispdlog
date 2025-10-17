#pragma once

#include "mispdlog/details/utils.h"
#include "mispdlog/level.h"
#include "mispdlog/sinks/base_sink.h"
#include <iostream>
#include <ostream>
#include <string_view>

namespace mispdlog {
namespace sinks {

template <typename Mutex> class console_sink : public base_sink<Mutex> {
protected:
  void sink_it_(const details::log_message &msg) override {
    std::string time_str = details::format_time(msg.time);
    std::string_view level_str = level_to_string(msg.level);
    std::cout << "[" << time_str << "]" << "[" << level_str << "]"
              << msg.payload << "\n";
  }

  void flush_() override { std::cout << std::flush; }
};

using console_sink_mt = console_sink<std::mutex>;
using console_sink_st = console_sink<null_mutex>;

template <typename Mutex> class stderr_sink : public base_sink<Mutex> {
protected:
  void sink_it_(const details::log_message &msg) override {
    std::string time_str = details::format_time(msg.time);
    std::string_view level_str = level_to_string(msg.level);
    std::cerr << "[" << time_str << "]" << "[" << level_str << "]"
              << msg.payload << "\n";
  }

  void flush_() override { std::cerr << std::flush; }
};

using stderr_sink_mt = stderr_sink<std::mutex>;
using stderr_sink_st = stderr_sink<null_mutex>;

} // namespace sinks
} // namespace mispdlog