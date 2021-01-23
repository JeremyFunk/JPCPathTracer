#pragma once

#include "core/ISampler.h"
#include "core/Linalg.h"
#ifdef _MSC_VER
    #include <vcruntime.h>
#endif
namespace jpc_tracer
{
    
    class DebugSampler : public ISampler
    {
    public:
        DebugSampler() = default;
        virtual Prec Get1DSample() override;
        virtual Vec2 Get2DSample() override;
        virtual Ref<ISampler> Clone() override;
        virtual void Get1DSampleArray(size_t dim, size_t sample_count, Prec* desination) override;
        virtual void Get2DSampleArray(size_t dim_y,size_t dim_x, size_t sample_count, Vec2* desination)  override;
    };
}