#include "DiscreteSampler.h"



namespace jpctracer::detail
{
    __DiscreateSamplerResult DiscreteSampler::Sample(const Prec& random_val) const
    {
        auto sampled_it = std::lower_bound(std::begin(m_weights_sum),
                            std::end(m_weights_sum),random_val);
            
        Prec lower_sum = 0;
        if(sampled_it>std::begin(m_weights_sum))
            lower_sum = *(sampled_it-1);
        Prec upper_sum = *sampled_it;

        Prec new_rand_val = (random_val-lower_sum)/(upper_sum-lower_sum);

        size_t sampled_idx = std::distance(std::begin(m_weights_sum),sampled_it); 

        return {sampled_idx,new_rand_val};
    }
    DiscreteSampler::DiscreteSampler(const Prec* weights,int size)
            : m_weights_sum(size,0)
    {
        assert(size>0);
        m_weights_sum[0] = weights[0];
        for(int i=1; i<size;i++)
        {
            m_weights_sum[i-1]+=weights[i];
        }
        
    }
}