#include "BoundingBoxIntersection.h"
#include "jpc_tracer/core/maths/Constants.h"



namespace jpctracer::raytracing
{
    std::optional<IntersectionInfo> BoundsIntersect(const Bound& bound, const Ray& ray, const Vec3& inverse_direction, const Int3 dir_is_negative[3]) 
    {        
        Prec x_min = (bound[    dir_is_negative[0]][0] - ray.Origin[0]) * inverse_direction[0];
        Prec x_max = (bound[1 - dir_is_negative[0]][0] - ray.Origin[0]) * inverse_direction[0];

        Prec y_min = (bound[    dir_is_negative[1]][1] - ray.Origin[1]) * inverse_direction[1];
        Prec y_max = (bound[1 - dir_is_negative[1]][1] - ray.Origin[1]) * inverse_direction[1];

        Prec z_min = (bound[    dir_is_negative[2]][2] - ray.Origin[2]) * inverse_direction[2];
        Prec z_max = (bound[1 - dir_is_negative[2]][2] - ray.Origin[2]) * inverse_direction[2];

        Vec3 min_distance = 

        if(x_min > y_max || y_min > x_max || x_min > z_max || z_min > x_max)
            return false;
            
        if(y_min > x_min)
            x_min = y_min;

        if(y_max < x_max)
            x_max = y_max;

        if(z_min > x_min)
           x_min = z_min;

        if(z_max < x_max)
            x_max = z_max;

        if(x_max <= 0 || ray.max_length  <= x_min)
            return false;
        
        return true;
    }
}