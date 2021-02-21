#pragma once
#include "Base.h"
#include "core/Linalg.h"
#include "core/Logger.h"
#include <array>
#include <cassert>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <tuple>
#include <type_traits>
#include <vector>
//#include<concepts>



namespace jpc_tracer
{
    template<class T,size_t n>
    T ImportanceSampling(const std::array<T,n>& vals,const std::array<Prec,n>& pdfs)
    {
        T result;
        for(int i= 0; i<n;i++) result+= vals[i]/pdfs[i];
        
        return result/n;
    }

    //template<class T,class ReturnT, class...Args >
    //concept Functor = std::is_convertible<T, std::function<ReturnT(Args... )>>::value;

    
    namespace MIS {
        
        template<
            class DistT,
            class DistRange,
            class PdfT,
            class InT,
            class RelativeCount
        >
        constexpr Prec BalanceHeuristic(
            DistT current_dist, 
            DistRange dist_range,
            PdfT pdf,
            InT x,
            RelativeCount rel_count
        )
        {
            auto summer = [&](Prec sum, DistT dist_idx) {return sum + pdf(dist_idx,x)* rel_count(dist_idx);};
            Prec sum = std::accumulate(std::begin(dist_range),std::end(dist_range),0.f,summer);
            auto p = pdf(current_dist,x);
            auto r = rel_count(current_dist);
            return p * r / sum;
        };

        enum class WeightType
        {
            BALANCE
        };

        template<
            class OutT,
            class SampleT,
            class SamplerT,
            class DistRange,
            class DistT,
            class RelativeCount,
            class FuncT, 
            class PdfT
        >
        constexpr OutT UpdateMISSum(
            OutT previous_sum, 
            SampleT sample,
            const SamplerT& sampler,
            DistRange dist_range,
            const DistT& current_dist,   
            const RelativeCount& relcount,   
            const FuncT& func, 
            const PdfT& pdf,
            const WeightType& weight
        )
        {
            auto x = sampler(sample);
            OutT f = func(x);
            Prec p = pdf(current_dist,x);
            Prec w = BalanceHeuristic(current_dist,dist_range,pdf,x,relcount);
            return previous_sum + f/p * w;
        };

        

        template<
            class SampleRange,
            class DistRange,
            class DistT,
            class OutT,
            class SamplerT,
            class FuncT, 
            class PdfT, 
            class RelativeCount
        >
        constexpr OutT MultipleImportanceSampling(
            SampleRange sample_range,
            DistRange dist_range,
            const DistT& current_dist,
            OutT start_val,
            const SamplerT& sampler,
            const FuncT& func, 
            const PdfT& pdf,
            const RelativeCount& relcount,
            const WeightType& weight
            )
        {
            using namespace std::placeholders;
            using SampleT = decltype(*std::begin(sample_range));            

            int sample_count = std::distance(std::begin(sample_range), std::end(sample_range));

            OutT sum = std::accumulate(std::begin(sample_range),std::end(sample_range), 
                start_val, 
                std::bind(
                    UpdateMISSum<OutT,SampleT,SamplerT,DistRange,DistT,RelativeCount,FuncT,PdfT>,
                     _1,_2,sampler,dist_range,current_dist,relcount,func,pdf,weight)
            );
            return sum / (Prec)sample_count;
        }

    }
    template<class ProbabilityRange>
    std::pair<int, Vec2> SampleDiscreteDistribution(Vec2 random_point, ProbabilityRange prob_range)
    {
        Prec sum = 0;
        assert(random_point[0] <= 1);
        assert(random_point[1] <= 1);
        assert(random_point[0] >= 0);
        assert(random_point[1] >= 0);
        int distribution_counter = 0;
        for(const auto& prob : prob_range )
        {
            if(sum + prob >= random_point[0])
            {
                //remap random_point
                random_point[0]-=sum;
                random_point[0] /= prob;

                return {distribution_counter,random_point};
            }
            distribution_counter++;
            sum+= prob;
        }
        assert(sum>=0.98);
        return {0,random_point};
        
    }
}