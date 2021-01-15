#pragma once

#include "core/ISampler.h"
#include "core/Linalg.h"
#include <random>
#include <vector>


namespace samplers
{

    class StratifiedSampler final: public core::ISampler
    {
    public:
        StratifiedSampler();
        virtual core::Prec Get1DSample() override;
        virtual core::Vec2 Get2DSample() override;
        virtual std::shared_ptr<core::ISampler> Clone() override;
        void Get1DSamples(size_t size,core::Prec* desination);
        void Get2DSamples(size_t size_x, size_t size_y,core::Vec2* desination);

        virtual void Get1DSampleArray(size_t dim, size_t sample_count, core::Prec* desination) override;
        virtual void Get2DSampleArray(size_t dim_y,size_t dim_x, size_t sample_count, core::Vec2* desination)  override;


    private:
        std::uniform_real_distribution<core::Prec> _generator;
        std::random_device _random_device; 
        std::mt19937 _gen;
        int64_t _samples_per_pixel;
    };
    


   
}