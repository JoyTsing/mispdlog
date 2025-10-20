#pragma once

#include "mispdlog/sinks/base_sink.h"
#include <fmt/format.h>
#include <iostream>
#include <ostream>

namespace mispdlog {
namespace sinks {

template <typename Mutex> class console_sink : public base_sink<Mutex> {
protected:
  void sink_it_(const details::log_message &msg) override {
    fmt::memory_buffer formatted;
    this->format_(msg, formatted);
    std::cout.write(formatted.data(), formatted.size()); // out
  }

  void flush_() override { std::cout << std::flush; }
};

using console_sink_mt = console_sink<std::mutex>;
using console_sink_st = console_sink<null_mutex>;

template <typename Mutex> class stderr_sink : public base_sink<Mutex> {
protected:
  void sink_it_(const details::log_message &msg) override {
    fmt::memory_buffer formatted;
    this->format_(msg, formatted);
    std::cerr.write(formatted.data(), formatted.size()); // out
  }

  void flush_() override { std::cerr << std::flush; }
};

using stderr_sink_mt = stderr_sink<std::mutex>;
using stderr_sink_st = stderr_sink<null_mutex>;

} // namespace sinks
} // namespace mispdlog