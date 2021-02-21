#pragma once

#include "core/Linalg.h"
#include "core/SurfaceInteraction.h"
namespace jpc_tracer
{
    Vec3 OffsetInteractionPoint(const SurfaceInteraction& interaction);

    Prec CosinusWeight(const Vec3& first_direction,const Vec3& second_direction);
    
}