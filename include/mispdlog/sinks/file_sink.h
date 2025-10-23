#pragma once

#include "mispdlog/details/log_message.h"
#include "mispdlog/sinks/base_sink.h"

#include <fmt/format.h>
#include <fstream>
#include <ios>
#include <mutex>
#include <stdexcept>

namespace mispdlog {
namespace sinks {
template <typename Mutex> class file_sink : public base_sink<Mutex> {
public:
  /**
   * @brief Construct a new file sink object
   *
   * @param filename
   * @param truncate true:overwrite, false:append
   */
  explicit file_sink(const std::string &filename, bool truncate = false) {
    std::ios::openmode mode = truncate ? std::ios::trunc : std::ios::app;
    file_.open(filename, std::ios::out | mode);
    if (file_.is_open() == false) {
      throw std::runtime_error("Failed to open file: " + filename);
    }
  }

  file_sink(file_sink &&) = delete;

  ~file_sink() {
    if (file_.is_open()) {
      file_.close();
    }
  }

protected:
  void sink_it_(const details::log_message &message) override {
    fmt::memory_buffer buf;
    this->format_(message, buf);
    // write
    file_.write(buf.data(), buf.size());
  }

  void flush_() override { file_.flush(); }

private:
  std::ofstream file_;
};

using file_sink_mt = file_sink<std::mutex>;
using file_sink_st = file_sink<null_mutex>;
} // namespace sinks
} // namespace mispdlog