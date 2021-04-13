#pragma once
#include <algorithm>
#include<array>
#include "jpc_tracer/core/core.h"

namespace jpctracer::shadersys
{
    struct __DiscreateSamplerResult
    {
        size_t SampledIdx;
        Prec ResampledPoint; 
    };



    class DiscreteSampler
    {
    public:
        //precondition weights.size>0
        DiscreteSampler(const Prec* weights,int size);
        //random_val in [0,1]
        __DiscreateSamplerResult Sample(const Prec& random_val) const;

    private:
        std::vector<Prec> m_weights_sum;
    };


}