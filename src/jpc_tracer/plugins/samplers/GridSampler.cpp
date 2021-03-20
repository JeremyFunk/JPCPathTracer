#include "jpc_tracer/plugins/samplers/GridSampler.h"
#include <random>

namespace jpctracer
{
    namespace sampler
    {
        RandOp::RandOp(std::random_device& rd) 
            : dist(0.0,0.9995), m_gen(rd())
        {
        }
    
        Prec RandOp::operator()() 
        {
            return dist(m_gen); 
        }
    }
}