#pragma once
#include "Base.h"
#include "core/Linalg.h"
#include "core/Logger.h"
#include <array>
#include <functional>
#include <iterator>
#include <numeric>
#include <vector>

namespace jpc_tracer
{
    template<class T,size_t n>
    T ImportanceSampling(const std::array<T,n>& vals,const std::array<Prec,n>& pdfs)
    {
        T result;
        for(int i= 0; i<n;i++) result+= vals[i]/pdfs[i];
        
        return result/n;
    }

    /*
    Vals    Pdf           Dist_Idx                    
    ....    ...|... |...  |

    relative_count 
    ....
    */

    template<class T>
    struct MISSamples
    {
        T Value;
        std::vector<Prec> Pdfs;
        int DistributionIdx; 

        MISSamples(T value, const std::vector<Prec>& pdfs, int idx);
    };

    template<class T>
    MISSamples<T>::MISSamples(T value, const std::vector<Prec>& pdfs, int idx) 
    : Value(value), Pdfs(pdfs), DistributionIdx(idx)
    {
        
    }

    //sample_count is the sample count per distribution

    template<class MISSampleT, class WeightFuncT>
    inline auto _MISUpdateSum(const MISSampleT& sample, Prec* inv_counts, Prec* relative_counts, WeightFuncT weight_func)
    {
        
        auto weight = weight_func(&sample.Pdfs[0],&sample.Pdfs[sample.Pdfs.size()],relative_counts,sample.DistributionIdx);
        return sample.Value/ sample.Pdfs[sample.DistributionIdx] * weight *inv_counts[sample.DistributionIdx];
    }

    template<class MISSampleIT , class WeightFuncT, class SampleValT>
    inline SampleValT MultipleImportanceSampling(MISSampleIT samples_begin,const MISSampleIT samples_end, 
    const int* sample_counts, const int distribution_count,SampleValT start_val, WeightFuncT weight_func) 
    {
        using namespace std::placeholders;
        int full_sample_count = std::accumulate(sample_counts,sample_counts+distribution_count,0);
        std::vector<Prec> inv_counts(distribution_count);
        std::vector<Prec> relative_counts(distribution_count);

        for(int i= 0; i< distribution_count;i++) {
            inv_counts[i] = 1.f/(Prec)sample_counts[i] ;
            relative_counts[i] = (Prec)sample_counts[i]/(Prec) full_sample_count ;
        }

        return std::accumulate(samples_begin,samples_end,start_val,
        [&](SampleValT sum, decltype(*samples_begin) samples){
            return sum + _MISUpdateSum(samples,&inv_counts[0],&relative_counts[0],weight_func);
            });
    }


    Prec BalanceHeuristic(const Prec* pdf_first,const Prec* pdf_last, Prec* relative_count,int distribution_idx);

}