#pragma once
#include "constants.h"
#include<memory.h>
#include <memory>
#include <utility>
#include "Logger.h"

namespace jpc_tracer {
    template<class T>
    using Ref = std::shared_ptr<T>;
    
    template<class T,class... ArgsT>
    Ref<T> MakeRef(ArgsT&& ... args)
    {
        return std::make_shared<T>(std::forward<ArgsT>(args)...);
    }

    template<class T>
    using Scope = std::unique_ptr<T>;

    template<class T,class... ArgsT>
    Scope<T> MakeScope(ArgsT&& ... args)
    {
        return std::make_unique<T>(std::forward<ArgsT>(args)...);
    }


}

