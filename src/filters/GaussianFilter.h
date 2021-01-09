#pragma once
#include "core/IFilter.h"
#include "core/Linalg.h"
namespace filters
{
    class GaussianFilter : public core::IFilter 
    {
    public:
        GaussianFilter(core::Prec standard_deviation = 2);
        float Evaluate(core::Vec2 point) override;
    private:
        core::Prec _std_derivative;
    };
}