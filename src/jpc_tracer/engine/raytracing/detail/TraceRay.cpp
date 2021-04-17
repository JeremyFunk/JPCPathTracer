#include "jpc_tracer/engine/raytracing/Base.h"
#include "jpc_tracer/engine/raytracing/TraceRay.h"
#include "Intersect.h"
#include "acceleration/IntersectionInfo.h"
#include "Scene.h"
#include <optional>
#include "jpc_tracer/engine/raytracing/detail/TracingContext.h"

#include "jpc_tracer/engine/raytracing/Base.h"

namespace jpctracer::raytracing {

    


    std::optional<SurfaceInteraction> TraceRay(Ray& ray,
        AnyHitCallBack any_hit_program, 
        const Scene* scene) 
    {

        IntersectionResult intersection = Intersect(*scene, ray, any_hit_program);

        if (intersection.interaction.has_value())
        {
            int a = 10;
        }
        
        if(intersection.ShouldTerminate)
            return std::nullopt;

        return intersection.interaction;

        
    }
}