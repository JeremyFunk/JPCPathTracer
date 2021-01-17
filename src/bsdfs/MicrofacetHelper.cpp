#include "MicrofacetHelper.h"

namespace bsdfs {
    core::Vec3 MicrofacetNormal(const core::Vec3& scattered_direction,const core::Vec3& incident_direction)
    {
        //See Microfacet Models for Refraction through Rough Surfaces 2007 Section 4.2;
        core::Vec3 half_direction = scattered_direction + incident_direction;
        half_direction.normalize();
        return half_direction;
    }
}