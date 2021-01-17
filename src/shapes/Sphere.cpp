#pragma once
#include "Sphere.h"
#include "core/Bounds3D.h"
#include "core/IShape.h"
#include "core/Linalg.h"
#include "core/SurfaceInteraction.h"
#include "utilities/arithmetic.h"
#include <iostream>
#include <optional>


namespace shapes
{
    Sphere::Sphere(const core::Vec3& pos, const core::Prec& radius, std::shared_ptr<const core::IMaterial> material)
    : _pos(pos), _radius(radius), _material(material)
    {}
    
    std::optional<core::IntersectionData> Sphere::Intersect(const core::Ray& ray) const 
    {   
        // reference: https://en.wikipedia.org/wiki/Line%E2%80%93sphere_intersection

        /* 
        *   basic formula:
        * 
        *   || ray.Origin + distance * ray.Direction - _pos||^2 = r^2
        */

        core::Vec3 ray_origin_to_pos = ray.Origin - _pos;

        core::Prec ray_dir_dot_ray_origin_to_pos = ray.Direction.dot(ray_origin_to_pos);

        // check how many solutions exist
        core::Prec checker = pow(ray_dir_dot_ray_origin_to_pos, 2);

        checker -= ray_origin_to_pos.squaredNorm() - pow(_radius, 2);

        if(checker < 0) // => no solution
        {
            return std::nullopt;
        }

        // calculate solutions

        // min 1 solution
        core::Prec distance = -ray_dir_dot_ray_origin_to_pos;

        if(checker > 0) // 2 solutions
        {
            checker = sqrt(checker);
            
            core::Prec distance_temp = distance - checker;

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
        // core::Vec3 intersection_point = ray.Origin + distance * ray.Direction;

        // core::Prec u = intersection_point.x() / intersection_point.norm();
        // core::Prec v = intersection_point.y() / intersection_point.norm();

        //output
        core::IntersectionData output;

        output.UV = core::Vec2::Zero();
        output.Distance = distance;
        output.Shape = (core::IShape*)this;

        return std::make_optional<core::IntersectionData>(output);


        // core::Prec t0, t1;
        // core::Vec3 L = _pos - ray.Origin;
        // core::Prec tca = L.dot(ray.Direction);
        // if(tca < 0) return std::nullopt;
        // core::Prec d2 = L.dot(L) - tca * tca;
        // if(d2 > _radius) return std::nullopt;
        // core::Prec thc = std::sqrt((_radius*_radius) - d2);
        // t0 = tca - thc;
        // t1 = tca + thc;

        // if(t0 > t1)
        //     std::swap(t0, t1);
        
        // if(t0 < 0){
        //     t0 = t1;
        //     if(t0 < 0)
        //         return std::nullopt;
        // }
        
        // core::IntersectionData output;

        // output.UV = core::Vec2::Zero();
        // output.Distance = t0;
        // output.Shape = (core::IShape*)this;

        // return std::make_optional<core::IntersectionData>(output);

        //Transform to 0 as origin
        // core::Vec3 base_transformed_origin = ray.Origin - _pos;

        // //quadratic equation setup
        // core::Prec a = ray.Direction.dot(ray.Direction);
        // core::Prec b = 2 * ray.Direction.dot(base_transformed_origin);
        // core::Prec c = base_transformed_origin.dot(base_transformed_origin) - _radius*_radius;

        // //solve at^2 + bt + c = 0 <=> t^2 + bt/a + c/a = 0
        // core::Prec solution1;
        // core::Prec solution2;
        // bool is_solved = utilities::solve_quadratic(a, b, c, &solution1, &solution2);

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
        // core::Vec3 intersection_vec = base_transformed_origin + (ray.Direction *  solution1);
        
        // //refined sphere-intersection
        // intersection_vec *= _radius / intersection_vec.norm();

        // if(intersection_vec.x() == 0 && intersection_vec.y() == 0)
        // {
        //     intersection_vec.x() = 1e-5f * _radius;
        // }

        // //output

        // core::IntersectionData output;
        // output.UV = core::Vec2::Zero();
        // output.Distance = solution1;
        // output.Shape = (core::IShape*)this;

        // return std::make_optional<core::IntersectionData>(output);
    }
    
    const core::SurfaceProperties Sphere::GetSurfaceProperties(const core::Ray& ray, const core::IntersectionData& data) const
    {
        core::SurfaceInteraction interaction;
        
        
        interaction.Point = ray.Direction * data.Distance + ray.Origin ;
        interaction.Normal = core::Vec3(interaction.Point-_pos).normalized();
        interaction.Distance = data.Distance;


        return core::SurfaceProperties(interaction,_material.get());
    }
    
    core::Bounds3D<core::Prec> Sphere::WorldBoundary() const 
    {
        return core::Bounds3D<core::Prec>({_pos.x()+_radius, _pos.y()+_radius, _pos.z()+_radius},
                                        {_pos.x()-_radius, _pos.y()-_radius, _pos.z()-_radius});
    }
    
    /*
    bool Sphere::IsIntersecting(const core::Ray& ray) const 
    {
        //Transform to 0 as origin
        core::Vec3 base_transformed_origin = ray.Origin - _pos;

        //quadratic equation setup
        core::Prec a = ray.Direction.dot(ray.Direction);
        core::Prec b = 2 * ray.Direction.dot(base_transformed_origin);
        core::Prec c = base_transformed_origin.dot(base_transformed_origin) - _radius*_radius;

        //solve at^2 + bt + c = 0 <=> t^2 + bt/a + c/a = 0
        core::Prec solution1;
        core::Prec solution2;
        bool is_solved = utilities::solve_quadratic(a, b, c, &solution1, &solution2);

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
    
    const core::IMaterial* Sphere::GetMaterial() const 
    {
        return _material.get();
    }
    
    */

}