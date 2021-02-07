#pragma once
#include "Base.h"
#include "core/Linalg.h"
#include <array>
#include <numeric>

namespace jpc_tracer
{
    template<class T,size_t n>
    T ImportanceSampling(const std::array<T,n>& funcs,const std::array<Prec,n>& pdfs)
    {
        T result;
        for(int i= 0; i<n;i++) result+= funcs[i]/pdfs[i];
        
        return result/n;
    }

}