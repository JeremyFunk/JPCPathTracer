#pragma once

#include "core/ISampler.h"
#include "core/Linalg.h"
#include <random>
#include <vector>


namespace samplers
{

    class StratifiedSampler : public core::ISampler
    {
    public:
        StratifiedSampler();
        virtual core::Prec Get1DSample() override;
        virtual core::Vec2 Get2DSample() override;
        virtual std::shared_ptr<core::ISampler> Clone() override;
        virtual std::unique_ptr< std::vector<core::Prec>> Get1DSampleArray(size_t size) override;
        virtual std::unique_ptr< std::vector<core::Vec2>> Get2DSampleArray(size_t size_x, size_t size_y) override;

    private:
        std::uniform_real_distribution<core::Prec> _generator;
        std::random_device _random_device; 
        std::mt19937 _gen;
        int64_t _samples_per_pixel;
    };
    


   
}