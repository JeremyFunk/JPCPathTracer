#pragma once
#include "core/Linalg.h"

#include <cmath>

namespace jpc_tracer
{
    inline bool solve_quadratic(const Prec& a, const Prec& b, const Prec& c, Prec* solution1, Prec* solution2) 
    {
        double discriminant = (double)b*(double)b - 4 * (double)a * (double)c;

        if(discriminant < 0)
            return false;

        double sqrt_discriminant = std::sqrt(discriminant);

        double q;

        if(b<0)
        {
            q = -0.5 * (b - sqrt_discriminant);
        }
        else
        {
            q = -0.5 * (b + sqrt_discriminant);
        }
        
        *solution1 = q / a;
        *solution2 = c / q;

        if(*solution1 > *solution2)
            std::swap(solution1, solution2);

        return true;
    }
}