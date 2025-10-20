#pragma once

#include <chrono>
#include <string>

namespace mispdlog {

// 别名
using string_view_t = std::string; // for c++11
using log_clock = std::chrono::high_resolution_clock;

// os
#ifdef _WIN32
#define MISPDLOG_WIN
#elif defined(__linux__)
#define MISPDLOG_LINUX
#else
#define MISPDLOG_UNKNOWN
#endif

// 导出符号定义为动态库做准备
#if defined(_WIN32) && defined(MISPDLOG_SHARED)
#ifdef MISPDLOG_BUILD
#define MISPDLOG_API __declspec(dllexport)
#else
#define MISPDLOG_API __declspec(dllimport)
#endif
#else
#define MISPDLOG_API
#endif

} // namespace mispdlog