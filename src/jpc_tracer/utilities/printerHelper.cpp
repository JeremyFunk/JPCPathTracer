#include "printerHelper.h"



namespace jpc_tracer
{
    void PrintRay(const Ray& ray) 
    {
        std::cout<<"Ray(Direction("<<ray.Direction[0]<<", "<<ray.Direction[1]<<", "<<ray.Direction[2]<<"), Origin("<<ray.Origin[0]<<", "<<ray.Origin[1]<<", "<<ray.Origin[2]<<"))\n";
    }
}