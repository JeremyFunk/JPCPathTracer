#include "raytracing_core/include/Base.h"
#include "raytracing_core/include/SurfaceInteraction.h"
#include "raytracing_core/include/TraceRay.h"
#include "raytracing_core/src/intersection/Intersect.h"
#include "raytracing_core/src/intersection/acceleration/IntersectionInfo.h"
#include "raytracing_core/src/shade_programs/ShadePrograms.h"
#include "raytracing_core/src/shade_programs/ShadingData.h"
#include <optional>
#include "raytracing_core/src/TracingContext.h"

namespace jpc_rt_core {

    


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