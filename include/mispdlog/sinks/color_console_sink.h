#pragma once

#include "mispdlog/details/log_message.h"
#include "mispdlog/sinks/base_sink.h"
#include <array>
#include <fmt/format.h>
#include <iostream>
#include <string>

namespace mispdlog {
namespace sinks {
// ANSI 颜色码
// 参考: https://en.wikipedia.org/wiki/ANSI_escape_code
namespace color {
constexpr const char *reset = "\033[0m";
constexpr const char *bold = "\033[1m";
constexpr const char *white = "\033[37m";
constexpr const char *green = "\033[32m";
constexpr const char *yellow = "\033[33m";
constexpr const char *red = "\033[31m";
constexpr const char *magenta = "\033[35m";
constexpr const char *cyan = "\033[36m";
} // namespace color

template <typename Mutex> class color_console_sink : public base_sink<Mutex> {
public:
  color_console_sink() {
    colors_[static_cast<int>(level::trace)] = color::white;
    colors_[static_cast<int>(level::debug)] = color::cyan;
    colors_[static_cast<int>(level::info)] = color::green;
    colors_[static_cast<int>(level::warn)] = color::yellow;
    colors_[static_cast<int>(level::error)] = color::red;
    colors_[static_cast<int>(level::critical)] =
        std::string(color::bold) + color::red;
  }

  ~color_console_sink() = default;

protected:
  void sink_it_(const details::log_message &message) override {
    fmt::memory_buffer buf;
    this->format_(message, buf);
    const std::string &prefix = colors_[static_cast<int>(message.level)];
    std::cout << prefix;
    std::cout.write(buf.data(), buf.size());
    std::cout << color::reset;
  }

  void flush_() override { std::cout << std::flush; }

private:
  std::array<std::string, 7> colors_;
};
using color_console_sink_mt = color_console_sink<std::mutex>;
using color_console_sink_st = color_console_sink<null_mutex>;

template <typename Mutex> class color_stderr_sink : public base_sink<Mutex> {
public:
  color_stderr_sink() {
    colors_[static_cast<int>(level::trace)] = color::white;
    colors_[static_cast<int>(level::debug)] = color::cyan;
    colors_[static_cast<int>(level::info)] = color::green;
    colors_[static_cast<int>(level::warn)] = color::yellow;
    colors_[static_cast<int>(level::error)] = color::red;
    colors_[static_cast<int>(level::critical)] =
        std::string(color::bold) + color::red;
  }

  ~color_stderr_sink() = default;

protected:
  void sink_it_(const details::log_message &message) override {
    fmt::memory_buffer buf;
    this->format_(message, buf);
    const std::string &prefix = colors_[static_cast<int>(message.level)];
    std::cerr << prefix;
    std::cerr.write(buf.data(), buf.size());
    std::cerr << color::reset;
  }

  void flush_() override { std::cerr << std::flush; }

private:
  std::array<std::string, 7> colors_;
};
using color_stderr_sink_mt = color_stderr_sink<std::mutex>;
using color_stderr_sink_st = color_stderr_sink<null_mutex>;
} // namespace sinks
} // namespace mispdlog