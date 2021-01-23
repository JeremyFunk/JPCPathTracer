#pragma once
#include"Base.h"
#include "Ray.h"
#include "ISampler.h"
#include <memory>
#ifdef _MSC_VER
    #include <vcruntime.h>
#endif
namespace jpc_tracer {
    class ICamera
    {
    public:
        virtual Ray GenerateRay(const Ref<ISampler>& sampler, Vec2i pixel) const = 0 ;
    };
}