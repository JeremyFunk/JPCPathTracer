#include "GaussianFilter.h"

#include "core/Bounds2D.h"
#include "core/IFilter.h"
#include "core/Linalg.h"
#include "core/constants.h"

namespace jpc_tracer
{

    Prec Gaussian(Prec x, Prec standard_deviation)
    {

        size_t kernel_length = ceil(standard_deviation) * 6 + 1; 
        
        const Prec sqrt2Pi = sqrt((Prec)2.0 * M_PI);

        Prec scaler = (standard_deviation * sqrt2Pi);

        Prec x_scaler = (2 * standard_deviation * standard_deviation);


        return exp(-(x * x) / x_scaler) / scaler;;
    }

    float GaussianFilter::Evaluate(Vec2 point) 
    {
        return Gaussian(point[0],_std_derivative)*Gaussian(point[0],_std_derivative);
    }

    GaussianFilter::GaussianFilter(Prec standard_deviation) 
        :  IFilter(Bounds2D<int>(ceil(standard_deviation*2),-ceil(standard_deviation*2),ceil(standard_deviation*2),-ceil(standard_deviation*2)))
        ,_std_derivative(standard_deviation) 
    
    {
    }
}
