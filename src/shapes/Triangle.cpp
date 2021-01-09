#include "TriangleMesh.h"
#include "core/constants.h"
#include <iostream>
#include <math.h>

namespace shapes
{
    Triangle::Triangle(std::shared_ptr<TriangleMesh> mesh, int index) : _mesh(mesh), _index(index)
    {}
    
    std::optional<core::IntersectionData> Triangle::Intersect(const core::Ray& ray) const
    {

        // //get point values
        // core::Vec3 v0 {(*_mesh->Vertices)[(*_mesh->Indices)[_index]*3], (*_mesh->Vertices)[(*_mesh->Indices)[_index]*3+1], (*_mesh->Vertices)[(*_mesh->Indices)[_index]*3+2]};
        // core::Vec3 v1 {(*_mesh->Vertices)[(*_mesh->Indices)[_index+3]*3], (*_mesh->Vertices)[(*_mesh->Indices)[_index+3]*3+1], (*_mesh->Vertices)[(*_mesh->Indices)[_index+3]*3+2]};
        // core::Vec3 v2 {(*_mesh->Vertices)[(*_mesh->Indices)[_index+6]*3], (*_mesh->Vertices)[(*_mesh->Indices)[_index+6]*3+1], (*_mesh->Vertices)[(*_mesh->Indices)[_index+6]*3+2]};
        // core::Vec3 v0v1 = v1-v0;
        // core::Vec3 v0v2 = v2-v0;


        // core::Vec3 N = v0v1.cross(v0v2);
        // core::Prec denom = N.dot(N);

        // core::Prec nDotDir = N.dot(ray.Direction);

        // if(abs(nDotDir) < ERROR_THICCNESS) return std::nullopt;

        // core::Prec d = N.dot(v0);
        // core::Prec t = (N.dot(ray.Origin) + d) / nDotDir;

        // if(t<ERROR_THICCNESS) return std::nullopt;

        // core::Vec3 p = ray.Origin + t * ray.Direction;
        // core::Vec3 edge0 = v1 - v0;
        // core::Vec3 vp0 = p - v0;
        // core::Vec3 c = edge0.cross(vp0);
        // if(N.dot(c) < ERROR_THICCNESS) return std::nullopt;


        
        // core::Vec3 edge1 = v2 - v1;
        // core::Vec3 vp1 = p - v1;
        // core::Vec3 c1 = edge1.cross(vp1);
        // core::Prec u = N.dot(c1);
        // if(u < ERROR_THICCNESS) return std::nullopt;


        
        // core::Vec3 edge2 = v0 - v2;
        // core::Vec3 vp2 = p - v2;
        // core::Vec3 c2 = edge2.cross(vp2);
        // core::Prec v = N.dot(c2);
        // if(v < ERROR_THICCNESS) return std::nullopt;

        // u /= denom;
        // v /= denom;


        
        // if(t < 0)
        //     std::cout<<" dsd";
        
        // core::IntersectionData output;
        // output.Distance = t;
        // output.Shape = (core::IShape*)this;
        // output.UV = {u,v};

        // return output;



















        //defines close to 0
        core::Prec epsilon = 0.00001;

        //get point values
        core::Vec3 p1 {(*_mesh->Vertices)[(*_mesh->Indices)[_index]*3], (*_mesh->Vertices)[(*_mesh->Indices)[_index]*3+1], (*_mesh->Vertices)[(*_mesh->Indices)[_index]*3+2]};
        core::Vec3 p2 {(*_mesh->Vertices)[(*_mesh->Indices)[_index+3]*3], (*_mesh->Vertices)[(*_mesh->Indices)[_index+3]*3+1], (*_mesh->Vertices)[(*_mesh->Indices)[_index+3]*3+2]};
        core::Vec3 p3 {(*_mesh->Vertices)[(*_mesh->Indices)[_index+6]*3], (*_mesh->Vertices)[(*_mesh->Indices)[_index+6]*3+1], (*_mesh->Vertices)[(*_mesh->Indices)[_index+6]*3+2]};

        core::Vec3 p2_transformed_to_p1 = p2-p1;
        core::Vec3 p3_transformed_to_p1 = p3-p1;

        core::Vec3 point_direction = ray.Direction.cross(p3_transformed_to_p1);
        core::Prec determinante = p2_transformed_to_p1.dot(point_direction);

        //check backfacing or never intersection
        if(determinante < epsilon)
            return std::nullopt;

        core::Prec inv_determinante = 1 / determinante;


        core::Vec3 ray_origin_transformed_to_p1 = ray.Origin - p1;

        //UV
        core::Prec u = ray_origin_transformed_to_p1.dot(point_direction) * inv_determinante;
        if(u < ERROR_THICCNESS || u > 1 - ERROR_THICCNESS)
            return std::nullopt;

        core::Vec3 cross_base_transformed_ray_p1p2 = ray_origin_transformed_to_p1.cross(p2_transformed_to_p1);
        core::Prec v = ray.Direction.dot(cross_base_transformed_ray_p1p2) * inv_determinante;
        if(v < ERROR_THICCNESS || u+v > 1 - ERROR_THICCNESS)
            return std::nullopt;
        
        //intersection point
        core::Prec intersection_point_distance = p3_transformed_to_p1.dot(cross_base_transformed_ray_p1p2) * inv_determinante;

        if(intersection_point_distance < 0)
            return std::nullopt;

        //output
        core::IntersectionData output;
        output.Distance = intersection_point_distance;
        output.Shape = (core::IShape*)this;
        output.UV = {u,v};
        
        return std::make_optional<core::IntersectionData>(output);
    }

    const core::SurfaceProperties Triangle::GetSurfaceProperties(const core::Ray& ray,const core::IntersectionData& data) const 
    {
        core::SurfaceInteraction interaction;
        
        interaction.Point = ray.Direction * data.Distance + ray.Origin ;
        interaction.Distance = data.Distance;
        core::Vec2 uv1 = {(*_mesh->UVs)[(*_mesh->Indices)[_index+1]*2], (*_mesh->UVs)[(*_mesh->Indices)[_index+1]*2+1]};
        core::Vec2 uv2 = {(*_mesh->UVs)[(*_mesh->Indices)[_index+4]*2], (*_mesh->UVs)[(*_mesh->Indices)[_index+4]*2+1]};
        core::Vec2 uv3 = {(*_mesh->UVs)[(*_mesh->Indices)[_index+7]*2], (*_mesh->UVs)[(*_mesh->Indices)[_index+7]*2+1]};

        
        core::Vec3 n1 = {(*_mesh->Normals)[(*_mesh->Indices)[_index+2]*3], (*_mesh->Normals)[(*_mesh->Indices)[_index+2]*3+1], (*_mesh->Normals)[(*_mesh->Indices)[_index+2]*3+2]};
        core::Vec3 n2 = {(*_mesh->Normals)[(*_mesh->Indices)[_index+5]*3], (*_mesh->Normals)[(*_mesh->Indices)[_index+5]*3+1], (*_mesh->Normals)[(*_mesh->Indices)[_index+5]*3+2]};
        core::Vec3 n3 = {(*_mesh->Normals)[(*_mesh->Indices)[_index+8]*3], (*_mesh->Normals)[(*_mesh->Indices)[_index+8]*3+1], (*_mesh->Normals)[(*_mesh->Indices)[_index+8]*3+2]};
        
        interaction.UV = core::LinAlg::Interpolate(uv1, uv2, uv3, data.UV[0], data.UV[1]);
        interaction.Normal = core::LinAlg::Interpolate(n1, n2, n3, data.UV[0], data.UV[1]);

        //interaction.Normal = n1;

        return core::SurfaceProperties(interaction,_mesh->Material.get());
    }
    
    core::Bounds3D<core::Prec> Triangle::WorldBoundary() const
    {
        core::Prec p1_x = (*_mesh->Vertices)[(*_mesh->Indices)[_index]*3];
        core::Prec p1_y = (*_mesh->Vertices)[(*_mesh->Indices)[_index]*3 + 1];
        core::Prec p1_z = (*_mesh->Vertices)[(*_mesh->Indices)[_index]*3 + 2];

        core::Prec p2_x = (*_mesh->Vertices)[(*_mesh->Indices)[_index + 3]*3];
        core::Prec p2_y = (*_mesh->Vertices)[(*_mesh->Indices)[_index + 3]*3 + 1];
        core::Prec p2_z = (*_mesh->Vertices)[(*_mesh->Indices)[_index + 3]*3 + 2];

        core::Prec p3_x = (*_mesh->Vertices)[(*_mesh->Indices)[_index + 6]*3];
        core::Prec p3_y = (*_mesh->Vertices)[(*_mesh->Indices)[_index + 6]*3 + 1];
        core::Prec p3_z = (*_mesh->Vertices)[(*_mesh->Indices)[_index + 6]*3 + 2];

        return core::Bounds3D<core::Prec>(std::max({p1_x, p2_x, p3_x}),
                                        std::min({p1_x, p2_x, p3_x}),
                                        std::max({p1_y, p2_y, p3_y}),
                                        std::min({p1_y, p2_y, p3_y}),
                                        std::max({p1_z, p2_z, p3_z}),
                                        std::min({p1_z, p2_z, p3_z}));

        return core::Bounds3D<core::Prec>();
    }

}