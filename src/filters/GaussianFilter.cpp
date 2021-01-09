#include "GaussianFilter.h"

#include "core/Bounds2D.h"
#include "core/IFilter.h"
#include "core/Linalg.h"
#include "core/constants.h"

namespace filters
{

    core::Prec Gaussian(core::Prec x, core::Prec standard_deviation)
    {

        size_t kernel_length = ceil(standard_deviation) * 6 + 1; 
        
        const core::Prec sqrt2Pi = sqrt((core::Prec)2.0 * M_PI);

        core::Prec scaler = (standard_deviation * sqrt2Pi);

        core::Prec x_scaler = (2 * standard_deviation * standard_deviation);


        return exp(-(x * x) / x_scaler) / scaler;;
    }

    float GaussianFilter::Evaluate(core::Vec2 point) 
    {
        return Gaussian(point[0],_std_derivative)*Gaussian(point[0],_std_derivative);
    }

    GaussianFilter::GaussianFilter(core::Prec standard_deviation) 
        :  core::IFilter(core::Bounds2D<int>(ceil(standard_deviation*2),-ceil(standard_deviation*2),ceil(standard_deviation*2),-ceil(standard_deviation*2)))
        ,_std_derivative(standard_deviation) 
    
    {
    }
}
