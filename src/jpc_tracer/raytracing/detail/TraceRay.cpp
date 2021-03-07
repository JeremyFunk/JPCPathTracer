#include "jpc_tracer/raytracing/Base.h"
#include "jpc_tracer/raytracing/SurfaceInteraction.h"
#include "jpc_tracer/raytracing/TraceRay.h"
#include "jpc_tracer/raytracing/detail/intersection/Intersect.h"
#include "jpc_tracer/raytracing/detail/intersection/acceleration/IntersectionInfo.h"
#include "jpc_tracer/raytracing/detail/shade_programs/ShadePrograms.h"
#include "jpc_tracer/raytracing/detail/shade_programs/ShadingData.h"
#include <optional>
#include "jpc_tracer/raytracing/detail/TracingContext.h"

namespace jpctracer::raytracing {

    


    void TraceRay(const Scene* scene,const Ray& ray, Payload* payload, TracingContext* context) 
    {
        ShadePrograms& programs = context->Programs;
        UpdateShadePrograms(scene->m_scene_data->Shading,ray, payload, &programs);

        IntersectionResult intersection = Intersect(scene->m_scene_data,ray,programs);
        
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