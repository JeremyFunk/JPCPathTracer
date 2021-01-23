#include "gtest/gtest.h"
#include <iostream>
#include <vector>
#include "core/Linalg.h"
#include "samplers/StratifiedSampler.h"
#include "iostream"

namespace jpc_tracer {

    TEST(stratified_sampler,Rand1D)
    {
        
        std::cout<<"Stratifed Sampler First Rand 1D\n";
        StratifiedSampler sampler;
        for(int i = 0; i < 100; i++);
           //std::cout<<sampler.Get1DSample()<<std::endl;

        std::cout<<"Stratifed Sampler Second Rand 1D\n";

        auto sampler2 = sampler.Clone();
        for(int i = 0; i < 100; i++);
            //std::cout<<sampler2->Get1DSample()<<std::endl;
        
    }

    TEST(stratified_sampler,Array1D)
    {
        StratifiedSampler sampler;
        auto samples = std::vector<Prec>(8);
        sampler.Get1DSampleArray(8,1,samples.data());
        for(const Prec& sample : samples)
        {
            EXPECT_TRUE(0 <= sample);
            EXPECT_TRUE(sample < 1);
            std::cout<<sample<<std::endl;;
        }
    }

    TEST(stratified_sampler,Array2D)
    {
        StratifiedSampler sampler;
        auto samples = std::vector<Vec2>(8*8);
        sampler.Get2DSampleArray(8,8,1,samples.data());
        for(const Vec2& sample : samples)
        {
            EXPECT_TRUE(0 <= sample[0]);
            EXPECT_TRUE(sample[0] < 1);
            EXPECT_TRUE(0 <= sample[1]);
            EXPECT_TRUE(sample[1] < 1);
            
            std::cout<<sample[0]<<","<<sample[1]<<std::endl;
        }
    }


}
