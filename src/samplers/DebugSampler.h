#pragma once

#include "core/ISampler.h"
#include <vcruntime.h>

namespace samplers
{
    
    class DebugSampler : public core::ISampler
    {
    public:
        DebugSampler() = default;
        virtual core::Prec Get1DSample() override;
        virtual core::Vec2 Get2DSample() override;
        virtual std::shared_ptr<core::ISampler> Clone() override;
        virtual std::unique_ptr<std::vector<core::Prec>> Get1DSampleArray(size_t size) override;
        virtual std::unique_ptr<std::vector<core::Vec2>> Get2DSampleArray(size_t size_x, size_t size_y) override;
    };
}