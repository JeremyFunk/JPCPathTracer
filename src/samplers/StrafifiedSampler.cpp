#include "StratifiedSampler.h"

namespace samplers {
     StratifiedSampler::StratifiedSampler()
        : _generator(0,1) , _gen(_random_device())
    {
        
    }
    

    core::Prec StratifiedSampler::Get1DSample() 
    {
        const core::Prec epsilon = 0.0001;
        return std::min(_generator(_gen),1-epsilon);
    }

    core::Vec2 StratifiedSampler::Get2DSample() 
    {
        return {Get1DSample(),Get1DSample()};
    }


    std::shared_ptr<core::ISampler> StratifiedSampler::Clone() 
    {
        return std::make_shared<StratifiedSampler>();
    }
    

    std::unique_ptr< std::vector<core::Prec>> StratifiedSampler::Get1DSampleArray(size_t size)
    {
        auto samples = std::make_unique<std::vector<core::Prec>>(size);
        for(int i= 0; i < size; i++)
            samples->operator[](i) = ((float)i+Get1DSample())/(float)(size);

        return samples;
    }
    

    std::unique_ptr<std::vector<core::Vec2>> StratifiedSampler::Get2DSampleArray(size_t size_x,size_t size_y) 
    {
        
        auto samples = std::make_unique<std::vector<core::Vec2>>(size_x*size_y);
        for(int y = 0; y < size_y; y++)
        {
            for(int x = 0; x < size_x; x++)
            {
                
                core::Vec2& sample = samples->operator[](y*size_y+x);
                sample[0] = ((core::Prec)x+ Get1DSample() )/ ((core::Prec)size_x);
                sample[1] = ((core::Prec)y+ Get1DSample() )/ ((core::Prec)size_y);
            }
        }
        return samples;
    }
}