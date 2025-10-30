#pragma once
#include "mispdlog/common.h"
#include "mispdlog/details/threadpool.h"
#include "mispdlog/level.h"
#include "mispdlog/logger.h"
#include <cstddef>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace mispdlog {
namespace details {
class threadpool;
} // namespace details

class MISPDLOG_API registry {
public:
  registry(const registry &) = delete;
  registry &operator=(const registry &) = delete;
  /**
   * @brief default level: info
   *
   * @return registry&
   */
  static registry &instance();

  /**
   * @brief throw std::runtime_error when logger exists.
   *
   * @param logger
   */
  void register_logger(std::shared_ptr<logger> logger);
  std::shared_ptr<logger> get(const std::string &logger_name);

  void drop(const std::string &logger_name);
  void drop_all();

  std::shared_ptr<logger> default_logger();
  void set_default_logger(std::shared_ptr<logger> new_default_logger);

public:
  void set_all_level(level level);
  void flush_all_loggers();

public:
  /**
   * @brief use it before async_logger
   *
   * @param queue_size
   * @param threads_n
   */
  void init_threadpool(size_t queue_size = 8192, size_t threads_n = 1);

  /**
   * @brief
   *
   * @return std::shared_ptr<details::threadpool>
   */
  std::shared_ptr<details::threadpool> threadpool();

  void set_threadpool(std::shared_ptr<details::threadpool> threadpool);

private:
  registry();
  ~registry() = default;
  /**
   * @brief default level: info
   *
   */
  void recover_default_();
  void throw_if_exists_(const std::string &logger_name);

  void create_default_threadpool_();

private:
  std::mutex mutex_;
  std::unordered_map<std::string, std::shared_ptr<logger>> mp_;
  std::shared_ptr<logger> default_logger_;
  std::shared_ptr<details::threadpool> threadpool_;
};
} // namespace mispdlog