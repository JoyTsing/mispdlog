#include "mispdlog/sinks/rotating_file_sink.h"
#include <cstddef>
#include <cstdio>
#include <fmt/format.h>
#include <memory>
#include <string>
#include <sys/stat.h>

namespace mispdlog {
namespace sinks {
template <typename Mutex>
rotating_file_sink<Mutex>::rotating_file_sink(const std::string &filename,
                                              size_t max_size, size_t max_files)
    : filename_(filename), max_size_(max_size), max_files_(max_files),
      current_size_(0), file_(nullptr) {
  if (max_size == 0) {
    throw std::invalid_argument("rotating_file_sink: max_size cannot be 0");
  }

  if (max_files == 0) {
    throw std::invalid_argument("rotating_file_sink: max_files cannot be 0");
  }

  auto real_filename = calc_filename(filename, 0);
  file_ = make_fileptr_(filename, "ab");

  if (file_ == nullptr) {
    throw std::runtime_error("rotating_file_sink: Failed to open file: " +
                             filename);
  }

  if (file_exists_(filename)) {
    current_size_ = file_size_(filename);
  }
}

template <typename Mutex>
std::shared_ptr<FILE>
rotating_file_sink<Mutex>::make_fileptr_(const std::string &filename,
                                         const std::string &mods) {
  return std::shared_ptr<FILE>(fopen(filename.c_str(), mods.c_str()),
                               [](FILE *fp) {
                                 if (fp) {
                                   std::fclose(fp);
                                 }
                               });
}

template <typename Mutex>
std::string rotating_file_sink<Mutex>::filename() const {
  return calc_filename(filename_, 0);
}

template <typename Mutex>
std::string
rotating_file_sink<Mutex>::calc_filename(const std::string &filename,
                                         size_t index) {
  if (index == 0) {
    return filename;
  }
  // log/log1.txt -> log/log1 + .txt
  size_t dot_pos = filename.find_last_of('.');
  size_t slash_pos = filename.find_last_of("/\\");

  if (dot_pos != std::string::npos &&
      (slash_pos == std::string::npos || dot_pos > slash_pos)) {
    std::string basename = filename.substr(0, dot_pos);
    std::string extend = filename.substr(dot_pos);
    return basename + "." + std::to_string(index) + extend;
  }
  // 没有扩展名
  return filename + "." + std::to_string(index);
}

template <typename Mutex>
void rotating_file_sink<Mutex>::sink_it_(const details::log_message &message) {
  fmt::memory_buffer buf;
  this->format_(message, buf);
  size_t message_size = buf.size();
  if (current_size_ + message_size > max_size_) {
    rotate_();
    current_size_ = 0;
  }
  if (file_) {
    fwrite(buf.data(), 1, buf.size(), file_.get());
    current_size_ += message_size;
  }
}

template <typename Mutex> void rotating_file_sink<Mutex>::flush_() {
  if (file_) {
    fflush(file_.get());
  }
}

template <typename Mutex> void rotating_file_sink<Mutex>::rotate_() {
  for (size_t i = max_files_; i > 0; i--) {
    std::string source_name = calc_filename(filename_, i - 1);
    if (file_exists_(source_name) == false) {
      continue;
    }
    std::string rotate_name = calc_filename(filename_, i);
    if (rename_file_(source_name, rotate_name) == false) {
      file_ = make_fileptr_(source_name, "wb");
      if (file_ == nullptr) {
        throw std::runtime_error("rotating_file_sink: Failed to reopen file "
                                 "after failed rotation: " +
                                 source_name);
      }
      return;
    }
  }
  // new log file
  std::string current_file = calc_filename(filename_, 0);
  file_ = make_fileptr_(current_file, "wb");
  if (file_ == nullptr) {
    throw std::runtime_error(
        "rotating_file_sink: Failed to create new file after rotation: " +
        current_file);
  }
}

template <typename Mutex>
bool rotating_file_sink<Mutex>::rename_file_(const std::string &src,
                                             const std::string &dst) {
  remove_file_(dst);
  return std::rename(src.c_str(), dst.c_str()) == 0;
}
template <typename Mutex>
bool rotating_file_sink<Mutex>::remove_file_(const std::string &filename) {
  return std::remove(filename.c_str());
}

template <typename Mutex>
bool rotating_file_sink<Mutex>::file_exists_(const std::string &filename) {
#ifdef _WIN32
  struct _stat buffer;
  return _stat(filename.c_str(), &buffer) == 0;
#else
  struct stat buffer;
  return stat(filename.c_str(), &buffer) == 0;
#endif
}

template <typename Mutex>
size_t rotating_file_sink<Mutex>::file_size_(const std::string &filename) {
#ifdef _WIN32
  struct _stat buffer;
  if (_stat(filename.c_str(), &buffer) != 0) {
    return 0;
  }
#else
  struct stat buffer;
  if (stat(filename.c_str(), &buffer) != 0) {
    return 0;
  }
#endif
  return static_cast<size_t>(buffer.st_size);
}

template class rotating_file_sink<std::mutex>;
template class rotating_file_sink<null_mutex>;
} // namespace sinks
} // namespace mispdlog