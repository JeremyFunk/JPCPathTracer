#pragma once
#include "jpc_tracer/core/maths/Spectrum.h"
#include "BsdfNode.h"

namespace jpctracer::shadersys
{
   
        
    template<class T>
    concept ShaderMethodContext = requires(ShaderContext& context,T func)
    {
        {func(context)};
    };
    template<class T>
    concept ShaderMethodFree = requires(T func)
    {
        {func()};
    } && ! ShaderMethodContext<T>;

        
    
    
    template<class T,class Result>
    concept ShaderFunction = requires(ShaderContext& context,T func)
    {
        {func(context)}->std::convertible_to<Result>;
    };
    

    template<class T>
    concept ShaderMethod = ShaderMethodContext<T>
                        || ShaderMethodFree<T>
                        || std::convertible_to<T,Spectrum>
                        || std::convertible_to<T,Prec>
                        || std::convertible_to<T,Vec3>;

    template<ShaderMethodContext T>
    constexpr auto Eval(ShaderContext& context, T func)
    {
        return func(context);
    }
    

    template<ShaderMethodFree T>
    constexpr auto Eval(ShaderContext& context, T func)
    {
        return func();
    }

    constexpr auto Eval(ShaderContext& context,const Spectrum& x)
    {
        return x;
    }
    constexpr auto Eval(ShaderContext& context,const Vec3& x)
    {
        return x;
    }
    constexpr auto Eval(ShaderContext& context,const std::convertible_to<Prec> auto& x)
    {
        return x;
    }


    template<class T,class... Args>
    concept __ShaderBindContext = requires(T f, Args&&... args,ShaderContext& context)
    {
        {f(context, Eval(context,std::forward<Args>(args))...)};
    };

    template<class T,class... Args>
    concept __ShaderBindFree = requires(T f, Args&&... args,ShaderContext& context)
    {
        {f(Eval(context,std::forward<Args>(args))...)};
    } && ! __ShaderBindContext<T,Args...>;


    template<class func, ShaderMethod... Args >
        requires __ShaderBindContext<func, Args...>
    inline constexpr auto ShaderBind(func&& f,Args&&... args)
    {
            //TypeDisplay<Args> args_t;
        return [f,... args = std::forward<Args>(args)](ShaderContext& context)
        {
            return f(context, Eval(context,args)...);
        };
    }

    template<class func, ShaderMethod... Args >
        requires __ShaderBindFree<func, Args...>
    inline constexpr auto ShaderBind(func&& f, Args&&... args)
    {
        return [f,... args = std::forward<Args>(args)](ShaderContext& context)
        {
            return f(Eval(context, args)...);
        };
    }

}