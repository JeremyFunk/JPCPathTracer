
#pragma once
#include "SceneBuilder.h"
#include <optional>

namespace jpctracer::raytracing {

    std::optional<SurfaceInteraction> TraceRay(const Ray& ray, AnyHitCallBack any_hit_program, 
        const Scene* scene);
}