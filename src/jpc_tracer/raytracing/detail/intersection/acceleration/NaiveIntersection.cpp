#include "NaiveIntersection.h"
#include "jpc_tracer/raytracing/Geometry.h"
#include "jpc_tracer/raytracing/Scene.h"
#include "jpc_tracer/raytracing/SurfaceInteraction.h"
#include "jpc_tracer/raytracing/detail/geometry/PrivateGeometry.h"
#include "jpc_tracer/raytracing/detail/intersection/acceleration/IntersectionInfo.h"
#include "jpc_tracer/raytracing/detail/intersection/acceleration/TriangleIntersection.h"
#include "jpc_tracer/raytracing/detail/shade_programs/ShadePrograms.h"
#include <optional>


namespace jpctracer::raytracing
{
    struct TriangleIdGeometry
    {
        int GeometryId;
        int TriangleId;
    };
    IntersectionResult NaiveIntersect(ShadePrograms& programs,const std::vector<PrivateGeometry>& geometries,const Ray& ray) 
    {
        std::optional<SurfaceInteraction> closest_interaction;
        for(const PrivateGeometry& geometry: geometries)
        {
            auto result = NaiveTriangleIntersect(programs,geometry,ray);
            if(result.ShouldTerminate)
                return IntersectionResult{std::nullopt,true};
            std::optional<SurfaceInteraction> new_interaction;
            closest_interaction = ClosestInteraction(closest_interaction,result.interaction);
        }
        return IntersectionResult{closest_interaction,false};
    }

    IntersectionResult NaiveTriangleIntersect(ShadePrograms& programs,const PrivateGeometry& geometry,const Ray& ray) 
    {

        std::optional<SurfaceInteraction> closest_interaction;

        int trianglecount = geometry.Triangles.MaterialIDs.size();
        for(int i = 0; i < trianglecount; i++)
        {
            Vec3 position = geometry.Triangles.Positions[i];
            Vec3 support_vec_1 = geometry.Triangles.SupportVec1[i];
            Vec3 support_vec_2 = geometry.Triangles.SupportVec2[i];

            auto current_info = TriangleIntersect(ray, position ,support_vec_1 , support_vec_2);

            if(current_info.has_value())
            {
                int material_id = geometry.Triangles.MaterialIDs[i];
                Vec3 n1 = geometry.Triangles.Normal1[i];
                Vec3 n2 = geometry.Triangles.Normal2[i];
                Vec3 n3 = geometry.Triangles.Normal3[i];

                Vec2 uv1 = geometry.Triangles.UV1[i];
                Vec2 uv2 = geometry.Triangles.UV2[i];
                Vec2 uv3 = geometry.Triangles.UV3[i];

                SurfaceInteraction interaction = TriangleGetInteraction(ray, current_info.value(), material_id, n1, n2, n3, uv1, uv2, uv3);

                AnyHitResult any_hit_result = programs.ExecuteAnyHit(interaction);

                if(any_hit_result.ShouldTerminate)
                    return IntersectionResult{std::nullopt,true};
                if(any_hit_result.IsHit)
                {
                    std::optional<SurfaceInteraction> new_interaction = ClosestInteraction(closest_interaction,std::make_optional(interaction));
                    closest_interaction = new_interaction;
                }
            }
        }
        return {closest_interaction,false};
    }

    IntersectionResult NaiveSphereIntersect(ShadePrograms& programs,const PrivateGeometry& geometry,const Ray& ray)
    {
        return IntersectionResult{};
    }
}