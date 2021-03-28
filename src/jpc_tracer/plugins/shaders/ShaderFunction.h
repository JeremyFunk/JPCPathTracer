#pragma once
#include "ShaderContext.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/plugins/shaders/bsdf/BsdfPool.h"

namespace jpctracer
{
    namespace detail {
        
        
        template<class T>
        concept ShaderMethodContext = requires(ShaderContext context,T func)
        {
            {func(context)};
        };
        template<class T>
        concept ShaderMethodFree = requires(T func)
        {
            {func()};
        } && ! ShaderMethodContext<T>;

        
    }
    
    template<class T,class Result>
    concept ShaderFunction = requires(ShaderContext context,T func)
    {
        {func(context)}->std::convertible_to<Result>;
    };
    

    template<class T>
    concept ShaderMethod = detail::ShaderMethodContext<T>
                        || detail::ShaderMethodFree<T>
                        || std::same_as<T,Spectrum>
                        || std::convertible_to<T,Prec>
                        || std::same_as<T,Vec3>;

    template<detail::ShaderMethodContext T>
    auto Eval(const ShaderContext& context,const T& func)
    {
        return func(context);
    }
    

    template<detail::ShaderMethodFree T>
    auto Eval(const ShaderContext& context,const T& func)
    {
        return func();
    }

    constexpr auto Eval(const ShaderContext& context,const Spectrum& x)
    {
        return x;
    }
    constexpr auto Eval(const ShaderContext& context,const Vec3& x)
    {
        return x;
    }
    constexpr auto Eval(const ShaderContext& context,const std::convertible_to<Prec> auto& x)
    {
        return x;
    }

    namespace detail {
        
        template<class T,class... Args>
        concept __ShaderBindContext = requires(T f, Args&&... args,const ShaderContext& context)
        {
            {f(context, Eval(context,std::forward<Args>(args))...)};
        };

        template<class T,class... Args>
        concept __ShaderBindFree = requires(T f, Args&&... args,const ShaderContext& context)
        {
            {f(Eval(context,std::forward<Args>(args))...)};
        } && ! __ShaderBindContext<T,Args...>;

    }
    template<class func, ShaderMethod... Args >
        requires detail::__ShaderBindContext<func, Args...>
    inline constexpr auto ShaderBind(const func& f, Args&&... args)
    {
        return [&](const ShaderContext& context)
        {
            return f(context, Eval(context,std::forward<Args>(args))...);
        };
    }

    template<class func, ShaderMethod... Args >
        requires detail::__ShaderBindFree<func, Args...>
    inline constexpr auto ShaderBind(const func& f, Args&&... args)
    {
        return [&](const ShaderContext& context)
        {
            return f(Eval(context,std::forward<Args>(args))...);
        };
    }

}