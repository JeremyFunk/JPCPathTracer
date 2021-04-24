#include "Logger.h"

namespace jpctracer
{
std::shared_ptr<spdlog::logger> Logger::_Logger;
bool Logger::_should_log = false;
void Logger::Init()
{
    if (!_should_log)
    {
        spdlog::set_pattern("%^[%T] %n: %v%$");
        _Logger = spdlog::stdout_color_mt("JPC Pathtracer");
        _Logger->set_level(spdlog::level::trace);
        _should_log = true;
        JPC_LOG_INFO("Start logger");
    }
}

} // namespace jpctracer
