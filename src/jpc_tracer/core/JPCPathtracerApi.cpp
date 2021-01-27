#pragma once
#include "JPCPathtracerApi.h"
#include "Logger.h"
#include <exception>
#include <stdexcept>
namespace jpc_tracer
{
    std::atomic<bool> JPCPathtracerApi::_already_started = false;
    
    JPCPathtracerApi::JPCPathtracerApi() 
    {
        Logger::Init();
        if(_already_started)
        {
            throw std::runtime_error("Only one instance of JPCPathtracerApi is allowed.");
        }else {
            _already_started = true;
        }
        
    }
    
    JPCPathtracerApi::~JPCPathtracerApi() 
    {
        _already_started = false;
    }
    
    bool JPCPathtracerApi::IsAlreadyStarted() const
    {
        return _already_started;
    }

    
}