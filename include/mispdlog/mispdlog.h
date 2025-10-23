#pragma once

#include "mispdlog/level.h"
#include "mispdlog/logger.h"
#include "mispdlog/registry.h"
#include "mispdlog/sinks/color_console_sink.h"
#include "mispdlog/sinks/console_sink.h"
#include "mispdlog/sinks/file_sink.h"
#include <memory>
#include <string>
#include <utility>

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
inline std::shared_ptr<logger> get_logger(const std::string &name) {
  return registry::instance().get(name);
}

/**
 * @brief throw throw std::runtime_error when logger exists.
 *
 * @param logger
 */
inline void register_logger(std::shared_ptr<logger> logger) {
  registry::instance().register_logger(logger);
}

inline void drop(const std::string &name) { registry::instance().drop(name); }

inline void drop_all() { registry::instance().drop_all(); }

inline std::shared_ptr<logger> default_logger() {
  return registry::instance().default_logger();
}

inline void set_default_logger(std::shared_ptr<logger> new_defauly_logger) {
  registry::instance().set_default_logger(new_defauly_logger);
}

inline void set_all_level(level level) {
  registry::instance().set_all_level(level);
}

inline void flush_all_loggers() { registry::instance().flush_all_loggers(); }

/**
 * @brief make color_console_sink_mt
 *
 * @param logger_name
 * @return std::shared_ptr<logger>
 */
inline std::shared_ptr<logger> stdout_color_mt(const std::string &logger_name) {
  auto sink = std::make_shared<sinks::color_console_sink_mt>();
  auto log = std::make_shared<logger>(logger_name, sink);
  register_logger(log);
  return log;
}

/**
 * @brief make color_stderr_sink_mt
 *
 * @param logger_name
 * @return std::shared_ptr<logger>
 */
inline std::shared_ptr<logger> stderr_color_mt(const std::string &logger_name) {
  auto sink = std::make_shared<sinks::color_stderr_sink_mt>();
  auto new_logger = std::make_shared<logger>(logger_name, sink);
  register_logger(new_logger);
  return new_logger;
}

/**
 * @brief make onsole_sink_mt
 *
 * @param logger_name
 * @return std::shared_ptr<logger>
 */
inline std::shared_ptr<logger> stdout_mt(const std::string &logger_name) {
  auto sink = std::make_shared<sinks::console_sink_mt>();
  auto new_logger = std::make_shared<logger>(logger_name, sink);
  register_logger(new_logger);
  return new_logger;
}

/**
 * @brief make sinks::file_sink_mt
 *
 * @param logger_name
 * @param path
 * @param truncate
 * @return std::shared_ptr<logger>
 */
inline std::shared_ptr<logger> basic_logger_mt(const std::string &logger_name,
                                               const std::string &path,
                                               bool truncate = false) {
  auto sink = std::make_shared<sinks::file_sink_mt>(path, truncate);
  auto new_logger = std::make_shared<logger>(logger_name, sink);
  register_logger(new_logger);
  return new_logger;
}

// fast use
template <typename... Args>
inline void trace(fmt::format_string<Args...> fmt, Args &&...args) {
  default_logger()->trace(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void debug(fmt::format_string<Args...> fmt, Args &&...args) {
  default_logger()->debug(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void info(fmt::format_string<Args...> fmt, Args &&...args) {
  default_logger()->info(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void warn(fmt::format_string<Args...> fmt, Args &&...args) {
  default_logger()->warn(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void error(fmt::format_string<Args...> fmt, Args &&...args) {
  default_logger()->error(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void critical(fmt::format_string<Args...> fmt, Args &&...args) {
  default_logger()->critical(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void off(fmt::format_string<Args...> fmt, Args &&...args) {
  default_logger()->off(fmt, std::forward<Args>(args)...);
}
} // namespace mispdlog