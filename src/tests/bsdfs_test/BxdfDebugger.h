#pragma once
#include "bsdfs/IBXDF.h"
#include "bsdfs/IMicrofacetDistribution.h"
#include "core/Linalg.h"
#include "core/Spectrum.h"
#include <cmath>
#include <memory>
#include <vector>


namespace bsdfs
{
    
    void BxdfDebugSampleUniform(IBXDF* bxdf, std::string name);
    void BxdfDebugSampleUniform(IBXDF* bxdf, std::string name, core::Vec3 income_vec);
    void DistDebugSampleUniform(IMicrofacetDistribution* dist, std::string name, core::Vec3 income_vec);

}