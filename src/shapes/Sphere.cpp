#pragma once
#include "Sphere.h"
#include "core/Bounds3D.h"
#include "core/IShape.h"
#include "core/Linalg.h"
#include "core/SurfaceInteraction.h"
#include "utilities/arithmetic.h"
#include <optional>


namespace shapes
{
    Sphere::Sphere(const core::Vec3& pos, const core::Prec& radius, std::shared_ptr<const core::IMaterial> material)
    : _pos(pos), _radius(radius), _material(material)
    {}
    
    std::optional<core::IntersectionData> Sphere::Intersect(const core::Ray& ray) const 
    {
        core::Prec t0, t1;
        core::Vec3 L = _pos - ray.Origin;
        core::Prec tca = L.dot(ray.Direction);
        if(tca < 0) return std::nullopt;
        core::Prec d2 = L.dot(L) - tca * tca;
        if(d2 > _radius) return std::nullopt;
        core::Prec thc = std::sqrt((_radius*_radius) - d2);
        t0 = tca - thc;
        t1 = tca + thc;

        if(t0 > t1)
            std::swap(t0, t1);
        
        if(t0 < 0){
            t0 = t1;
            if(t0 < 0)
                return std::nullopt;
        }
        
        core::IntersectionData output;

        output.UV = core::Vec2::Zero();
        output.Distance = t0;
        output.Shape = (core::IShape*)this;

        return std::make_optional<core::IntersectionData>(output);

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