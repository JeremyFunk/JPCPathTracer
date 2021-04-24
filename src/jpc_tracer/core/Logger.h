#pragma once
#include "memory"
#include <cassert>
// Inspired by chernos Hazel
#pragma warning(push, 0)
#include "spdlog/sinks/stdout_color_sinks.h"
#include <spdlog/spdlog.h>
#pragma warning(pop)

namespace jpctracer
{
class Logger
{
  public:
    static void Init();

    static std::shared_ptr<spdlog::logger>& GetLogger()
    {
        return _Logger;
    }
    static bool _should_log;

  private:
    static std::shared_ptr<spdlog::logger> _Logger;
};

// Debug
template <class T> class TypeDisplay;

} // namespace jpctracer

#define jpc_assertm(exp, msg) assert(((void)msg, exp))

#ifndef JPC_LOG_ACTIVATED
#define JPC_LOG_ACTIVATED 1
#endif

#if JPC_LOG_ACTIVATED
  // log macros
#define JPC_LOG_TRACE(...)                                                                                             \
    if (::jpctracer::Logger::_should_log)                                                                              \
    ::jpctracer::Logger::GetLogger()->trace(__VA_ARGS__)
#define JPC_LOG_INFO(...)                                                                                              \
    if (::jpctracer::Logger::_should_log)                                                                              \
    ::jpctracer::Logger::GetLogger()->info(__VA_ARGS__)
#define JPC_LOG_WARN(...)                                                                                              \
    if (::jpctracer::Logger::_should_log)                                                                              \
    ::jpctracer::Logger::GetLogger()->warn(__VA_ARGS__)
#define JPC_LOG_ERROR(...)                                                                                             \
    if (::jpctracer::Logger::_should_log)                                                                              \
    ::jpctracer::Logger::GetLogger()->error(__VA_ARGS__)
#define JPC_LOG_CRITICAL(...)                                                                                          \
    if (::jpctracer::Logger::_should_log)                                                                              \
    ::jpctracer::Logger::GetLogger()->critical(__VA_ARGS__)
#define JPC_DISPLAY_TYPE(...) ::jpctracer::TypeDisplay<decltype(__VA_ARGS__)>()
#else
#define JPC_LOG_TRACE(...)
#define JPC_LOG_INFO(...)
#define JPC_LOG_WARN(...)
#define JPC_LOG_ERROR(...)
#define JPC_LOG_CRITICAL(...)
#define JPC_DISPLAY_TYPE(...)
#endif