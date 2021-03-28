#include "DistributionFactory.h"



namespace jpctracer
{

    DistributionFactory DistributionFactory::Clone() 
    {
        return DistributionFactory();
    }
}