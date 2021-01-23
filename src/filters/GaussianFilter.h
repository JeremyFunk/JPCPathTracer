#pragma once
#include "core/IFilter.h"
#include "core/Linalg.h"
namespace jpc_tracer
{
    class GaussianFilter : public IFilter 
    {
    public:
        GaussianFilter(Prec standard_deviation = 2);
        float Evaluate(Vec2 point) override;
    private:
        Prec _std_derivative;
    };
}