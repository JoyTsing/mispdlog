#pragma once

#include "mispdlog/details/log_message.h"
#include "mispdlog/sinks/base_sink.h"
#include <cstddef>
#include <cstdio>
#include <memory>
#include <mutex>
#include <string>

namespace mispdlog {
namespace sinks {

/**
 * @brief rotating_file_sink: 按大小滚动的文件 Sink
 * 轮转策略:
 * - mylog.txt 达到 max_size 后轮转
 * - mylog.txt → mylog.1.txt (重命名)
 * - mylog.1.txt → mylog.2.txt (如果存在)
 * - 创建新的 mylog.txt
 * - 当达到 max_files 限制时,删除最旧的文件
 * @tparam Mutex
 */
template <typename Mutex> class rotating_file_sink : public base_sink<Mutex> {
public:
  rotating_file_sink(const std::string &filename, size_t max_size,
                     size_t max_files);

  ~rotating_file_sink() = default;
  std::string filename() const;

  /**
   * @brief calcuate filename by index
   *
   * @param filename
   * @param index
   * @return std::string
   */
  static std::string calc_filename(const std::string &filename, size_t index);

protected:
  void sink_it_(const details::log_message &message) override;
  void flush_() override;

private:
  void rotate_();
  bool rename_file_(const std::string &src, const std::string &dst);
  bool remove_file_(const std::string &filename);
  bool file_exists_(const std::string &filename);
  size_t file_size_(const std::string &filename);

  std::shared_ptr<FILE> make_fileptr_(const std::string &filename,
                                      const std::string &mods);

private:
  std::string filename_;
  size_t max_size_;
  size_t max_files_;
  size_t current_size_;
  std::shared_ptr<FILE> file_;
};

using rotating_file_sink_mt = rotating_file_sink<std::mutex>;
using rotating_file_sink_st = rotating_file_sink<null_mutex>;

} // namespace sinks
} // namespace mispdlog