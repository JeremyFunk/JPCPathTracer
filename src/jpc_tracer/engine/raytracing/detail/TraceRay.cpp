#include "jpc_tracer/engine/raytracing/Base.h"
#include "jpc_tracer/engine/raytracing/SurfaceInteraction.h"
#include "jpc_tracer/engine/raytracing/TraceRay.h"
#include "Intersect.h"
#include "acceleration/IntersectionInfo.h"
#include "ShadePrograms.h"
#include "Scene.h"
#include <optional>
#include "jpc_tracer/engine/raytracing/detail/TracingContext.h"

namespace jpctracer::raytracing {

    


    void TraceRay(const Ray& ray, Payload* payload, TracingContext* context) 
    {
        ShadePrograms& programs = context->programs;
        const Scene& scene = *context->scene; 
        UpdateShadePrograms(scene,ray, payload, programs);

        IntersectionResult intersection = Intersect(scene,ray,programs);
        
        if(intersection.ShouldTerminate)
            return;

        std::optional<SurfaceInteraction>& interaction = intersection.interaction;

        if(intersection.interaction.has_value())
        {
            programs.ExecuteClosestHit(interaction.value());
        }else {
            programs.ExecuteMiss();
        }
    }
}