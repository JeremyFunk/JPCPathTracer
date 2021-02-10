#include "MonteCarlo.h"
#include <numeric>



namespace jpc_tracer
{
    

    Prec BalanceHeuristic(const Prec* pdf_first,const Prec* pdf_last, Prec* relative_count,int distribution_idx) 
    {
        Prec sum = std::inner_product(pdf_first,pdf_last,relative_count,0.f);

        return pdf_first[distribution_idx]*relative_count[distribution_idx] / sum;
    }
}