#pragma once
#include "SurfaceInteraction.h"
#include "IMaterial.h"

namespace core {
    
    struct SurfaceProperties
    {
        SurfaceProperties(const SurfaceInteraction& interaction,
            const IMaterial* material
        ) : Interaction(interaction), Material(material) {}
        SurfaceInteraction Interaction;
        const IMaterial* Material;
    };
}