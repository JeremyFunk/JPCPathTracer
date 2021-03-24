#pragma once
#include <algorithm>
#include<array>
#include "jpc_tracer/core/core.h"

namespace jpctracer::shader::detail 
{
    struct __DiscreateSamplerResult
    {
        size_t SampledIdx;
        Prec ResampledPoint; 
    };



    template<size_t _Size>
    class DiscreteSampler
    {
    public:
        DiscreteSampler(const std::array<Prec,_Size>& weights)
        {
            m_weights_sum.fill(0);
            m_weights_sum[0] = weights[0];
            for(int i=1; i<_Size;i++)
            {
                m_weights_sum[i-1]+=weights[i];
            }
            
        }
        //random_val in [0,1]
        __DiscreateSamplerResult Sample(const Prec& random_val)
        {
            size_t sampled_idx = std::lower_bound(std::begin(m_weights_sum),
                            std::end(m_weights_sum),random_val);
            
            Prec lower_sum = 0;
            if(sampled_idx>0)
                lower_sum = m_weights_sum[sampled_idx-1];
            Prec upper_sum = m_weights_sum[sampled_idx];

            Prec new_rand_val = (random_val-lower_sum)/(upper_sum-lower_sum);

            return {sampled_idx,new_rand_val};
        
        }


    private:
        std::array<Prec,_Size> m_weights_sum;
    };

    template<>
    class DiscreteSampler<0>
    {
    public:
        DiscreteSampler(const std::array<Prec,0>& weights)
        {

            
        }
        //random_val in [0,1]
        __DiscreateSamplerResult Sample(const Prec& random_val)
        {
        
            return {0,0};
        
        }

    };
}