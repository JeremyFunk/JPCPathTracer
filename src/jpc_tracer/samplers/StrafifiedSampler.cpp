#include "StratifiedSampler.h"
#include "core/Linalg.h"
#include <algorithm>
#include <memory>

namespace jpc_tracer {
     StratifiedSampler::StratifiedSampler()
        : _generator(0,1) , _gen(_random_device())
    {
        
    }
    

    Prec StratifiedSampler::Get1DSample() 
    {
        const Prec epsilon = 0.0001;
        return std::min(_generator(_gen),1-epsilon);
    }

    Vec2 StratifiedSampler::Get2DSample() 
    {
        return {Get1DSample(),Get1DSample()};
    }


    std::shared_ptr<ISampler> StratifiedSampler::Clone() 
    {
        return std::make_shared<StratifiedSampler>();
    }
    

    void StratifiedSampler::Get1DSamples(size_t size,Prec* desination)
    {

        for(int i= 0; i < size; i++)
            desination[i] = ((float)i+Get1DSample())/(float)(size);

    }
    

    void StratifiedSampler::Get2DSamples(size_t size_x,size_t size_y,Vec2* desination) 
    {
        
        for(int y = 0; y < size_y; y++)
        {
            for(int x = 0; x < size_x; x++)
            {
                
                Vec2& sample = desination[y*size_y+x];
                sample[0] = ((Prec)x+ Get1DSample() )/ ((Prec)size_x);
                sample[1] = ((Prec)y+ Get1DSample() )/ ((Prec)size_y);
            }
        }
    }


    void StratifiedSampler::Get1DSampleArray(size_t dim, size_t sample_count,Prec* desination) 
    {
        auto dim_slice = std::make_unique<std::vector<Prec>> (dim);
        for(int sample_idx = 0; sample_idx < sample_count; sample_idx++)
        {
            Get1DSamples(dim,dim_slice->data());
            auto random_func = std::default_random_engine();
            std::shuffle(dim_slice->begin(),dim_slice->end(), random_func);
            // std::random_shuffle(dim_slice->begin(),dim_slice->end());
            for(int dim_idx = 0; dim_idx<dim;dim_idx++)
                desination[sample_idx*sample_count+dim_idx] = dim_slice->operator[](dim_idx); 
        }
    }


    void StratifiedSampler::Get2DSampleArray(size_t dim_y,size_t dim_x, size_t sample_count,Vec2* desination) 
    {
        auto dim_slice = std::make_unique<std::vector<Vec2>> (dim_y*dim_x);
        for(int sample_idx = 0; sample_idx < sample_count; sample_idx++)
        {
            Get2DSamples(dim_x,dim_y,dim_slice->data());
            auto random_func = std::default_random_engine();
            std::shuffle(dim_slice->begin(),dim_slice->end(), random_func);
            // std::random_shuffle(dim_slice->begin(),dim_slice->end());
            for(int dim_y_idx = 0; dim_y_idx<dim_x;dim_y_idx++)
                for(int dim_x_idx = 0; dim_x_idx<dim_x;dim_x_idx++)
                    desination[dim_y_idx*dim_x*sample_count+ dim_x_idx*sample_count+sample_idx] 
                        = dim_slice->operator[](dim_y_idx*dim_x + dim_x_idx); 
        }
    }


}