#pragma once
#include "Ray.h"
#include "ISampler.h"
#include <memory>
#include <vcruntime.h>
namespace core {
    class ICamera
    {
    public:
        virtual Ray GenerateRay(const std::shared_ptr<ISampler>& sampler, Vec2i pixel) const = 0 ;
    };
}