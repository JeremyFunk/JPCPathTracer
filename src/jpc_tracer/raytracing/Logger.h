#pragma once
#include "memory"
//Inspired by chernos Hazel
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include "spdlog/sinks/stdout_color_sinks.h"
#pragma warning(pop)

namespace jpctracer::raytracing
{
    class Logger
    {
    public:
        static void Init();

		static std::shared_ptr<spdlog::logger>& GetLogger() { return _Logger; }
		static bool _should_log;
    private:
        static std::shared_ptr<spdlog::logger> _Logger;
    };

	    //Debug
    template<class T>
    class TypeDisplay;

    
}

#ifndef JPC_LOG_ACTIVATED
	#define JPC_LOG_ACTIVATED 1
#endif

#if JPC_LOG_ACTIVATED
	//log macros
	#define JPC_LOG_TRACE(...)    if(::jpctracer::raytracing::Logger::_should_log) ::jpctracer::raytracing::Logger::GetLogger()->trace(__VA_ARGS__)
	#define JPC_LOG_INFO(...)     if(::jpctracer::raytracing::Logger::_should_log) ::jpctracer::raytracing::Logger::GetLogger()->info(__VA_ARGS__)
	#define JPC_LOG_WARN(...)     if(::jpctracer::raytracing::Logger::_should_log) ::jpctracer::raytracing::Logger::GetLogger()->warn(__VA_ARGS__)
	#define JPC_LOG_ERROR(...)    if(::jpctracer::raytracing::Logger::_should_log) ::jpctracer::raytracing::Logger::GetLogger()->error(__VA_ARGS__)
	#define JPC_LOG_CRITICAL(...) if(::jpctracer::raytracing::Logger::_should_log) ::jpctracer::raytracing::Logger::GetLogger()->critical(__VA_ARGS__)
	#define JPC_DISPLAY_TYPE(...) ::jpctracer::raytracing::TypeDisplay<decltype(__VA_ARGS__)>()
#else
	#define JPC_LOG_TRACE(...)    
	#define JPC_LOG_INFO(...)     
	#define JPC_LOG_WARN(...)     
	#define JPC_LOG_ERROR(...)    
	#define JPC_LOG_CRITICAL(...) 
	#define JPC_DISPLAY_TYPE(...)
#endif