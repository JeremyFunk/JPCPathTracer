#pragma once
#include"Base.h"
#include "SurfaceInteraction.h"
#include "IMaterial.h"

namespace jpc_tracer {
    
    struct SurfaceProperties
    {
        SurfaceProperties(const SurfaceInteraction& interaction,
            const IMaterial* material
        ) : Interaction(interaction), Material(material) {}
        SurfaceInteraction Interaction;
        const IMaterial* Material;
    };
}