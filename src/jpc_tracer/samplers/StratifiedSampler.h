#pragma once

#include "core/ISampler.h"
#include "core/Linalg.h"
#include <random>
#include <vector>


namespace jpc_tracer
{

    class StratifiedSampler final: public ISampler
    {
    public:
        StratifiedSampler();
        virtual Prec Get1DSample() override;
        virtual Vec2 Get2DSample() override;
        virtual Ref<ISampler> Clone() override;
        void Get1DSamples(size_t size,Prec* desination);
        void Get2DSamples(size_t size_x, size_t size_y,Vec2* desination);

        virtual void Get1DSampleArray(size_t dim, size_t sample_count, Prec* desination) override;
        virtual void Get2DSampleArray(size_t dim_y,size_t dim_x, size_t sample_count, Vec2* desination)  override;


    private:
        std::uniform_real_distribution<Prec> _generator;
        std::random_device _random_device; 
        std::mt19937 _gen;
        int64_t _samples_per_pixel;
    };
    


   
}