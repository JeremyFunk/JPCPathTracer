#include "DistributionFactory.h"



namespace jpctracer::shadersys
{

    DistributionFactory DistributionFactory::Clone() 
    {
        return DistributionFactory();
    }
}