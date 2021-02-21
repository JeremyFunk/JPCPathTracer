#include "Sphere.h"
#include "core/Bounds3D.h"
#include "core/IShape.h"
#include "core/Linalg.h"
#include "core/SurfaceInteraction.h"
#include "utilities/arithmetic.h"
#include <iostream>
#include <optional>


namespace jpc_tracer
{
    Sphere::Sphere(const Vec3& pos, const Prec& radius, Ref<const IMaterial> material)
    : _pos(pos), _radius(radius), _material(material)
    {}
    
    std::optional<IntersectionData> Sphere::Intersect(const Ray& ray) const 
    {   
        // reference: https://en.wikipedia.org/wiki/Line%E2%80%93sphere_intersection

        /* 
        *   basic formula:
        * 
        *   || ray.Origin + distance * ray.Direction - _pos||^2 = r^2
        */

        Vec3 ray_origin_to_pos = ray.Origin - _pos;

        Prec ray_dir_dot_ray_origin_to_pos = ray.Direction.dot(ray_origin_to_pos);

        // check how many solutions exist
        Prec checker = std::pow(ray_dir_dot_ray_origin_to_pos, 2);

        checker -= ray_origin_to_pos.squaredNorm() - std::pow(_radius, 2);

        if(checker < 0) // => no solution
        {
            return std::nullopt;
        }

        // calculate solutions

        // min 1 solution
        Prec distance = -ray_dir_dot_ray_origin_to_pos;

        if(checker > 0) // 2 solutions
        {
            checker = std::sqrt(checker);
            
            Prec distance_temp = distance - checker;

            distance += checker;

            //choose smallest solution
            if(distance > distance_temp)
                distance = distance_temp;

        }

        // ray points in other direction 
        if(distance <= 0)
        {
            return std::nullopt;
        }

        // UV
        
        // intersection point
        // Vec3 intersection_point = ray.Origin + distance * ray.Direction;
        // Vec3 intersction_pos_point = intersection_point - _pos;

        // Prec u = (std::atan2(-intersction_pos_point.z(), intersction_pos_point.x()) + M_PI) / (2 * M_PI);
        // Prec v = std::acos(-(intersction_pos_point.y())/_radius) / M_PI;

        // u = 0.5 + std::atan2(intersction_pos_point.x(), intersction_pos_point.z()) / (2*M_PI);
        // v = 0.5 - std::asin(intersction_pos_point.y()) / M_PI;

        //output
        IntersectionData output;

        // output.UV = {u,v};
        output.UV = {0,0};
        output.Distance = distance;
        output.Shape = (IShape*)this;

        return std::make_optional<IntersectionData>(output);


        // Prec t0, t1;
        // Vec3 L = _pos - ray.Origin;
        // Prec tca = L.dot(ray.Direction);
        // if(tca < 0) return std::nullopt;
        // Prec d2 = L.dot(L) - tca * tca;
        // if(d2 > _radius) return std::nullopt;
        // Prec thc = std::sqrt((_radius*_radius) - d2);
        // t0 = tca - thc;
        // t1 = tca + thc;

        // if(t0 > t1)
        //     std::swap(t0, t1);
        
        // if(t0 < 0){
        //     t0 = t1;
        //     if(t0 < 0)
        //         return std::nullopt;
        // }
        
        // IntersectionData output;

        // output.UV = Vec2::Zero();
        // output.Distance = t0;
        // output.Shape = (IShape*)this;

        // return std::make_optional<IntersectionData>(output);

        //Transform to 0 as origin
        // Vec3 base_transformed_origin = ray.Origin - _pos;

        // //quadratic equation setup
        // Prec a = ray.Direction.dot(ray.Direction);
        // Prec b = 2 * ray.Direction.dot(base_transformed_origin);
        // Prec c = base_transformed_origin.dot(base_transformed_origin) - _radius*_radius;

        // //solve at^2 + bt + c = 0 <=> t^2 + bt/a + c/a = 0
        // Prec solution1;
        // Prec solution2;
        // bool is_solved = solve_quadratic(a, b, c, &solution1, &solution2);

        // if(is_solved == false)
        //     return std::nullopt;

        // //check if intersection
        
        // if(solution1 < 0)
        // {
        //     if(solution2 < 0)
        //     {
        //         return std::nullopt;
        //     } 
        //     else
        //     {
        //         solution1 = solution2;
        //     }
            
        // }
        // //solution1 is the only possible solution from this point on forward

        // //calculate intersection point
        // Vec3 intersection_vec = base_transformed_origin + (ray.Direction *  solution1);
        
        // //refined sphere-intersection
        // intersection_vec *= _radius / intersection_vec.norm();

        // if(intersection_vec.x() == 0 && intersection_vec.y() == 0)
        // {
        //     intersection_vec.x() = 1e-5f * _radius;
        // }

        // //output

        // IntersectionData output;
        // output.UV = Vec2::Zero();
        // output.Distance = solution1;
        // output.Shape = (IShape*)this;

        // return std::make_optional<IntersectionData>(output);
    }
    
    const SurfaceProperties Sphere::GetSurfaceProperties(const Ray& ray, const IntersectionData& data) const
    {
        SurfaceInteraction interaction;
        
        
        interaction.Point = ray.Direction * data.Distance + ray.Origin ;
        interaction.Normal = Vec3(interaction.Point-_pos).normalized();
        interaction.Distance = data.Distance;


        return SurfaceProperties(interaction,_material.get());
    }
    
    Vec3 Sphere::getCenter() const 
    {
        return _pos;
    }
    
    Bounds3D<Prec> Sphere::WorldBoundary() const 
    {
        return Bounds3D<Prec>({_pos.x()+_radius, _pos.y()+_radius, _pos.z()+_radius},
                                        {_pos.x()-_radius, _pos.y()-_radius, _pos.z()-_radius});
    }
    
    std::vector<Ref<const IMaterial>> Sphere::GetMaterials() const
    {
        return {_material};
    }
    
    /*
    bool Sphere::IsIntersecting(const Ray& ray) const 
    {
        //Transform to 0 as origin
        Vec3 base_transformed_origin = ray.Origin - _pos;

        //quadratic equation setup
        Prec a = ray.Direction.dot(ray.Direction);
        Prec b = 2 * ray.Direction.dot(base_transformed_origin);
        Prec c = base_transformed_origin.dot(base_transformed_origin) - _radius*_radius;

        //solve at^2 + bt + c = 0 <=> t^2 + bt/a + c/a = 0
        Prec solution1;
        Prec solution2;
        bool is_solved = solve_quadratic(a, b, c, &solution1, &solution2);

        if(!is_solved)
            return false;
        
        if(solution1 < 0)
        {
            if(solution2 < 0)
            {
                return false;
            }
        }

        return true;
    }
    
    const IMaterial* Sphere::GetMaterial() const 
    {
        return _material.get();
    }
    
    */

}