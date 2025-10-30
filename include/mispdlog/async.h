#pragma once

#include "mispdlog/async_logger.h"
#include "mispdlog/registry.h"
#include "mispdlog/sinks/color_console_sink.h"
#include "mispdlog/sinks/console_sink.h"
#include "mispdlog/sinks/file_sink.h"
#include "mispdlog/sinks/rotating_file_sink.h"

namespace mispdlog {

inline void init_thread_pool(size_t queue_size, size_t threads_n = 1) {
  registry::instance().init_threadpool(queue_size, threads_n);
}

inline std::shared_ptr<details::threadpool> thread_pool() {
  return registry::instance().threadpool();
}

inline std::shared_ptr<async_logger> async_stdout_color_mt(
    const std::string &logger_name,
    async_overflow_policy overflow_policy = async_overflow_policy::block) {
  auto sink = std::make_shared<sinks::color_console_sink_mt>();
  auto tp = registry::instance().threadpool();
  auto new_logger =
      std::make_shared<async_logger>(logger_name, sink, tp, overflow_policy);
  registry::instance().register_logger(new_logger);
  return new_logger;
}

inline std::shared_ptr<async_logger> async_stderr_color_mt(
    const std::string &logger_name,
    async_overflow_policy overflow_policy = async_overflow_policy::block) {
  auto sink = std::make_shared<sinks::color_stderr_sink_mt>();
  auto tp = registry::instance().threadpool();
  auto new_logger =
      std::make_shared<async_logger>(logger_name, sink, tp, overflow_policy);
  registry::instance().register_logger(new_logger);
  return new_logger;
}

inline std::shared_ptr<async_logger> async_stdout_mt(
    const std::string &logger_name,
    async_overflow_policy overflow_policy = async_overflow_policy::block) {
  auto sink = std::make_shared<sinks::console_sink_mt>();
  auto tp = registry::instance().threadpool();
  auto new_logger =
      std::make_shared<async_logger>(logger_name, sink, tp, overflow_policy);
  registry::instance().register_logger(new_logger);
  return new_logger;
}

inline std::shared_ptr<async_logger> async_file_mt(
    const std::string &logger_name, const std::string &filename,
    bool truncate = false,
    async_overflow_policy overflow_policy = async_overflow_policy::block) {
  auto sink = std::make_shared<sinks::file_sink_mt>(filename, truncate);
  auto tp = registry::instance().threadpool();
  auto new_logger =
      std::make_shared<async_logger>(logger_name, sink, tp, overflow_policy);
  registry::instance().register_logger(new_logger);
  return new_logger;
}

inline std::shared_ptr<async_logger> async_rotating_logger_mt(
    const std::string &logger_name, const std::string &filename,
    size_t max_size, size_t max_files,
    async_overflow_policy overflow_policy = async_overflow_policy::block) {
  auto sink = std::make_shared<sinks::rotating_file_sink_mt>(filename, max_size,
                                                             max_files);
  auto tp = registry::instance().threadpool();
  auto new_logger =
      std::make_shared<async_logger>(logger_name, sink, tp, overflow_policy);
  registry::instance().register_logger(new_logger);
  return new_logger;
}

template <typename Sink, typename... SinkArgs>
inline std::shared_ptr<async_logger>
create_async(const std::string &logger_name,
             async_overflow_policy overflow_policy, SinkArgs &&...sink_args) {
  auto sink = std::make_shared<Sink>(std::forward<SinkArgs>(sink_args)...);
  auto tp = registry::instance().threadpool();
  return std::make_shared<async_logger>(logger_name, sink, tp, overflow_policy);
}

template <typename Sink, typename... SinkArgs>
inline std::shared_ptr<async_logger>
create_async(const std::string &logger_name, SinkArgs &&...sink_args) {
  return create_async<Sink>(logger_name, async_overflow_policy::block,
                            std::forward<SinkArgs>(sink_args)...);
}

} // namespace mispdlog