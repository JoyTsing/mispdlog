#pragma once
#include "mispdlog/common.h"
#include "mispdlog/level.h"
#include "mispdlog/logger.h"
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

/**
┌─────────────────────────────────────────────────────────────────┐
│                    用户代码层                                    │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  方式1: 全局接口              方式2: 工厂函数                   │
│  minispdlog::info("msg")      auto log = stdout_color_mt("app") │
│  minispdlog::error("err")     log->info("msg")                  │
│         ↓                              ↓                         │
│  default_logger()              register_logger(log)             │
└───────────┬───────────────────────────┬──────────────────────────┘
            │                           │
            └───────────┬───────────────┘
                        ↓
         ┌──────────────────────────────────────┐
         │    minispdlog::registry (单例)       │
         ├──────────────────────────────────────┤
         │  mutex_ (线程安全锁)                 │
         ├──────────────────────────────────────┤
         │  loggers_:                            │
         │    unordered_map<string, logger_ptr> │
         │    ┌──────────────────────────┐      │
         │    │ "app"    -> app_logger   │      │
         │    │ "db"     -> db_logger    │      │
         │    │ "network"-> net_logger   │      │
         │    └──────────────────────────┘      │
         ├──────────────────────────────────────┤
         │  default_logger_: shared_ptr<logger> │
         │    ↓                                  │
         │  [彩色控制台 logger]                 │
         └──────────────────────────────────────┘
                        ↓
         ┌──────────────────────────────────────┐
         │           Logger 对象                 │
         │  ┌────────────────────────┐           │
         │  │ name: "app"            │           │
         │  │ sinks: [console_sink]  │           │
         │  │ level: info            │           │
         │  └────────────────────────┘           │
         └──────────────────────────────────────┘
 */
namespace mispdlog {
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

private:
  registry();
  /**
   * @brief default level: info
   *
   */
  void recover_default_();
  void throw_if_exists_(const std::string &logger_name);

private:
  std::mutex mutex_;
  std::unordered_map<std::string, std::shared_ptr<logger>> mp_;
  std::shared_ptr<logger> default_logger_;
};
} // namespace mispdlog