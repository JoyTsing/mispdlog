#pragma once

#include "mispdlog/details/threadpool.h"
#include "mispdlog/logger.h"

#include <memory>
#include <vector>

namespace mispdlog {

namespace details {
class threadpool;
} // namespace details

enum class async_overflow_policy {
  block,         //
  overrun_oldest //
};

class MISPDLOG_API async_logger final
    : public logger,
      public std::enable_shared_from_this<async_logger> {
  friend class details::threadpool;

public:
  // 构造函数:单个 Sink
  async_logger(std::string name, sinks::sink_ptr single_sink,
               std::weak_ptr<details::threadpool> tp,
               async_overflow_policy policy = async_overflow_policy::block);

  // 构造函数:多个 Sink
  async_logger(std::string name, std::vector<sinks::sink_ptr> sinks,
               std::weak_ptr<details::threadpool> tp,
               async_overflow_policy policy = async_overflow_policy::block);

  ~async_logger() override = default;

  // 禁止拷贝
  async_logger(const async_logger &) = delete;
  async_logger &operator=(const async_logger &) = delete;

protected:
  // 重写 logger 的虚函数:将消息 post 到队列(非阻塞返回)
  void sink_it_(const details::log_message &msg) override;

  // 重写 flush:向队列 post 刷新请求
  void flush_() override;

  void backend_sink_it_(const details::log_message &msg);

  // 后台线程调用:真正执行刷新
  void backend_flush_();

private:
  std::weak_ptr<details::threadpool> threadpool_;
  async_overflow_policy overflow_policy_;
};
} // namespace mispdlog