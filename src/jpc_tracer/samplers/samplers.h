#pragma once
#include "jpc_tracer/maths/maths.h"
#include <iterator>
#include "GridSampler.h"
#include "jpc_tracer/Concepts.h"
namespace jpctracer{
    namespace sampler {

        template<class T, class ResultIterator>
        concept SamplerFunction2D = std::invocable<T,UInt2,ResultIterator> 
            && std::output_iterator<ResultIterator,Vec2>;

        template<class T, class ResultIterator>
        concept SamplerFunction3D = std::invocable<T,UInt3,ResultIterator>
            && std::output_iterator<ResultIterator,Vec3>;


    }
    template<class T, class ResultIterator>
    concept SamplerBuilder2D = requires(T builder, ResultIterator iter)
    {
        {Build(builder)}->sampler::SamplerFunction2D<ResultIterator>;
    };

    template<class T, class ResultIterator>
    concept SamplerBuilder3D = requires(T builder, ResultIterator iter)
    {
        {Build(builder)}->sampler::SamplerFunction3D<ResultIterator>;
    };
    
    template<class T,class ResultIt2D,class ResultIt3D>
    concept SamplerBuilder = SamplerBuilder2D<T, ResultIt2D> && SamplerBuilder3D<T, ResultIt3D>;

    static_assert(SamplerBuilder<StratifiedSamplerBuilder,Vec2*,Vec3*>);
    
    static_assert(SamplerBuilder<DebugSamplerBuilder,Vec2*,Vec3*>);

    

}