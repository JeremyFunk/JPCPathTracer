#pragma once

#include "core/ISampler.h"
#include "core/Linalg.h"
#ifdef _MSC_VER
    #include <vcruntime.h>
#endif
namespace samplers
{
    
    class DebugSampler : public core::ISampler
    {
    public:
        DebugSampler() = default;
        virtual core::Prec Get1DSample() override;
        virtual core::Vec2 Get2DSample() override;
        virtual std::shared_ptr<core::ISampler> Clone() override;
        virtual void Get1DSampleArray(size_t dim, size_t sample_count, core::Prec* desination) override;
        virtual void Get2DSampleArray(size_t dim_y,size_t dim_x, size_t sample_count, core::Vec2* desination)  override;
    };
}