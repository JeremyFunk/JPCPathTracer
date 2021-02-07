#include "Integration.h"
#include "core/SurfaceInteraction.h"



namespace jpc_tracer
{
    Vec3 OffsetInteractionPoint(const SurfaceInteraction& interaction) 
    {
        return interaction.Point + interaction.Normal* ERROR_THICCNESS;
    }
    
    Prec CosinusWeight(const Vec3& first_direction,const Vec3& second_direction) 
    {
        return std::abs(first_direction.dot(second_direction));
    }
}