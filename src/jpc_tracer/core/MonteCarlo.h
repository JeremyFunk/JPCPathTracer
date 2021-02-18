#pragma once
#include "Base.h"
#include "core/Linalg.h"
#include "core/Logger.h"
#include <array>
#include <functional>
#include <iterator>
#include <numeric>
#include <tuple>
#include <type_traits>
#include <vector>
#include<concepts>



namespace jpc_tracer
{
    template<class T,size_t n>
    T ImportanceSampling(const std::array<T,n>& vals,const std::array<Prec,n>& pdfs)
    {
        T result;
        for(int i= 0; i<n;i++) result+= vals[i]/pdfs[i];
        
        return result/n;
    }

    template<class T,class ReturnT, class...Args >
    concept Functor = std::is_convertible<T, std::function<ReturnT(Args... )>>::value;

    namespace MIS {
        template<class T,class InT>
        concept Pdf = Functor<T, Prec, InT>;

        template<class T, class OutT, class InT>
        concept Func = Functor<T,OutT,InT>;

        template<class T,class InT, class SampleT>
        concept Sampler = Functor<T, InT,SampleT>;

        template<class T,class InT,class DistributionT, class SampleT>
        concept DistSampler = Functor<T, InT,DistributionT,SampleT>;

        template<class T, class InT, class DistributionT>
        concept DistPdf = Functor<T,Prec,DistributionT,InT>;

        template<class T, class DistributionT, class OutT, class InT>
        concept DistFunc = Functor<T,DistributionT,OutT,InT>;

        template<class T, class DistributionT>
        concept DistRelativeCount = Functor<T,Prec,DistributionT>;

        template<class T,class DistributionT, class InT, class DistributionRangeT,class DistRelativeCountT,class DistPdfT>
        concept Weight = Functor<T,Prec,DistributionT,InT,DistributionRangeT,DistPdfT,DistRelativeCountT> 
            && DistPdf<DistPdfT,InT,DistributionT>
            && DistRelativeCount<DistRelativeCountT, DistributionT>
            && std::ranges::forward_range<DistributionRangeT>;
        
        template<class InT, std::ranges::forward_range DistributionRangeT, DistPdf<InT,int> DistPdfT, DistRelativeCount<int> DistRelativeCountT>
        constexpr Prec BalanceHeuristic(int current_dist, InT x, DistributionRangeT dist_range,
        DistPdfT dist_pdf, DistRelativeCountT dist_relative_count)
        {
            
            auto summer = [&](Prec sum, int dist_idx) {return sum + dist_pdf(dist_idx,x)* dist_relative_count(dist_idx);};
            Prec sum = std::accumulate(std::ranges::begin(dist_range),std::ranges::end(dist_range),0.f,summer);

            return dist_pdf(current_dist,x)* dist_relative_count(current_dist) / sum;
        }

        

        template<
            class DistributionT, 
            class InT, 
            class OutT,
            class SampleT, 
            std::ranges::forward_range DistributionRangeT, 
            DistSampler<InT,DistributionT,SampleT> SamplerT,
            DistFunc<DistributionT,OutT,InT> DistFuncT, 
            DistPdf<InT,DistributionT> DistPdfT, 
            DistRelativeCount<DistributionT> DistRelativeCountT,
            Weight<DistributionT, InT, DistributionRangeT, DistRelativeCountT,DistPdfT> WeightT
 
        > 
        constexpr OutT MultipleImportanceSampling(OutT start_val, DistributionRangeT dist_range, SampleT* samples,int sample_count , SamplerT dist_sampler, 
        DistFuncT dist_func, DistPdfT dist_pdf, DistRelativeCountT dist_relative_count, WeightT weight)
        {
            auto update_sum = [&](OutT sum, SampleT sample, DistributionT dist_idx) {
                auto x = dist_sampler(dist_idx,sample);
                auto f = dist_func(dist_idx,x);
                auto p = dist_pdf(dist_idx,x);
                auto w = weight(dist_idx,x,dist_range,dist_pdf,dist_relative_count);
                return sum + f/p * w;
            };

            auto compute_dist_integral = [&](std::pair<OutT,SampleT*> sum, DistributionT dist_idx) -> std::pair<OutT,SampleT*>
            {
                OutT integral = sum.first; 
                SampleT* current_sample = sum.second;
                int dist_sample_count = dist_relative_count(dist_idx)*sample_count;
                using namespace std::placeholders;
                OutT dist_sum = std::accumulate(current_sample,current_sample+dist_sample_count,start_val, std::bind(update_sum,_1,_2,dist_idx));

                return std::pair{integral+ dist_sum / (Prec) dist_sample_count, current_sample+dist_sample_count};
            };

            return std::accumulate(std::ranges::begin(dist_range),std::ranges::end(dist_range),
                std::pair{start_val,samples},
                compute_dist_integral ).first;


        }
    }
}