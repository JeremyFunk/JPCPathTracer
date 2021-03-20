#include "TriangleIntersection.h"
#include "jpc_tracer/engine/raytracing/Base.h"
#include "jpc_tracer/engine/raytracing/detail/intersection/acceleration/IntersectionInfo.h"



namespace jpctracer::raytracing
{
    /*
    p1 = position
    support_vec_1 = p2-p1
    support_vec_2 = p3-p1
    */
    std::optional<IntersectionInfo> TriangleIntersect(const Ray& ray, Vec3 position, Vec3 support_vec_1,Vec3 support_vec_2) 
    {
        assert((ray.Direction.norm()-1.f)<0.001);
        //defines close to 0
        Prec epsilon = 0.00001;


        Vec3 point_direction = ray.Direction.cross(support_vec_2);
        Prec determinante = support_vec_1.dot(point_direction);

        //check backfacing or never intersection
        if(determinante < epsilon)
            return std::nullopt;

        Prec inv_determinante = 1 / determinante;


        Vec3 diff_origin_position = ray.Origin - position;

        //UV
        Prec u = diff_origin_position.dot(point_direction) * inv_determinante;

        Vec3 cross_op_s1 = diff_origin_position.cross(support_vec_1);
        Prec v = ray.Direction.dot(cross_op_s1) * inv_determinante;
        

        if(u < ERROR_THICCNESS || u > 1 - ERROR_THICCNESS || v < ERROR_THICCNESS || u+v > 1 - ERROR_THICCNESS) 
            return std::nullopt;
        
        //intersection point
        Prec intersection_point_distance = support_vec_2.dot(cross_op_s1) * inv_determinante;

        if(intersection_point_distance < 0)
            return std::nullopt;

        //output
        IntersectionInfo output;
        output.Distance = intersection_point_distance;
        output.UV = {u,v};
        
        return std::make_optional(output);
    }

    Vec3 Interpolate(Vec3 v1, Vec3 v2, Vec3 v3, Prec u, Prec v){
        return u * v2 + v * v3 + (1 - u - v) * v1;
    }
    Vec2 Interpolate(Vec2 v1, Vec2 v2, Vec2 v3, Prec u, Prec v){
        return u * v2 + v * v3 + (1 - u - v) * v1;
    }
    
    SurfaceInteraction TriangleGetInteraction(const Ray& ray, IntersectionInfo info,int material_id, Vec3 normal_1,Vec3 normal_2,Vec3 normal_3,Vec2 uv_1,Vec2 uv_2,Vec2 uv_3) 
    {
        
        
        Vec3 point = ray.Direction * info.Distance + ray.Origin ;
        Prec distance = info.Distance;
       
        
        Vec2 uv = Interpolate(uv_1, uv_2, uv_3, info.UV[0], info.UV[1]);
        Vec3 normal = Interpolate(normal_1, normal_2, normal_3, info.UV[0], info.UV[1]);

        SurfaceInteraction interaction = {point,normal,uv,distance,material_id};
        return interaction;
    }


}