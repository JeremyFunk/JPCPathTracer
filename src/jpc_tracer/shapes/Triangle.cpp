#include "TriangleMesh.h"
#include "core/constants.h"
#include <iostream>
#include <math.h>

namespace jpc_tracer
{
    Triangle::Triangle(Ref<TriangleMesh> mesh, int index) : _mesh(mesh), _index(index)
    {}
    
    std::optional<IntersectionData> Triangle::Intersect(const Ray& ray) const
    {

        // //get point values
        // Vec3 v0 {(*_mesh->Vertices)[(*_mesh->Indices)[_index]*3], (*_mesh->Vertices)[(*_mesh->Indices)[_index]*3+1], (*_mesh->Vertices)[(*_mesh->Indices)[_index]*3+2]};
        // Vec3 v1 {(*_mesh->Vertices)[(*_mesh->Indices)[_index+3]*3], (*_mesh->Vertices)[(*_mesh->Indices)[_index+3]*3+1], (*_mesh->Vertices)[(*_mesh->Indices)[_index+3]*3+2]};
        // Vec3 v2 {(*_mesh->Vertices)[(*_mesh->Indices)[_index+6]*3], (*_mesh->Vertices)[(*_mesh->Indices)[_index+6]*3+1], (*_mesh->Vertices)[(*_mesh->Indices)[_index+6]*3+2]};
        // Vec3 v0v1 = v1-v0;
        // Vec3 v0v2 = v2-v0;


        // Vec3 N = v0v1.cross(v0v2);
        // Prec denom = N.dot(N);

        // Prec nDotDir = N.dot(ray.Direction);

        // if(abs(nDotDir) < ERROR_THICCNESS) return std::nullopt;

        // Prec d = N.dot(v0);
        // Prec t = (N.dot(ray.Origin) + d) / nDotDir;

        // if(t<ERROR_THICCNESS) return std::nullopt;

        // Vec3 p = ray.Origin + t * ray.Direction;
        // Vec3 edge0 = v1 - v0;
        // Vec3 vp0 = p - v0;
        // Vec3 c = edge0.cross(vp0);
        // if(N.dot(c) < ERROR_THICCNESS) return std::nullopt;


        
        // Vec3 edge1 = v2 - v1;
        // Vec3 vp1 = p - v1;
        // Vec3 c1 = edge1.cross(vp1);
        // Prec u = N.dot(c1);
        // if(u < ERROR_THICCNESS) return std::nullopt;


        
        // Vec3 edge2 = v0 - v2;
        // Vec3 vp2 = p - v2;
        // Vec3 c2 = edge2.cross(vp2);
        // Prec v = N.dot(c2);
        // if(v < ERROR_THICCNESS) return std::nullopt;

        // u /= denom;
        // v /= denom;


        
        // if(t < 0)
        //     std::cout<<" dsd";
        
        // IntersectionData output;
        // output.Distance = t;
        // output.Shape = (IShape*)this;
        // output.UV = {u,v};

        // return output;



















        //defines close to 0
        Prec epsilon = 0.00001;

        //get point values
        Vec3 p1 {(*_mesh->Vertices)[(*_mesh->Indices)[_index]*3], (*_mesh->Vertices)[(*_mesh->Indices)[_index]*3+1], (*_mesh->Vertices)[(*_mesh->Indices)[_index]*3+2]};
        Vec3 p2 {(*_mesh->Vertices)[(*_mesh->Indices)[_index+3]*3], (*_mesh->Vertices)[(*_mesh->Indices)[_index+3]*3+1], (*_mesh->Vertices)[(*_mesh->Indices)[_index+3]*3+2]};
        Vec3 p3 {(*_mesh->Vertices)[(*_mesh->Indices)[_index+6]*3], (*_mesh->Vertices)[(*_mesh->Indices)[_index+6]*3+1], (*_mesh->Vertices)[(*_mesh->Indices)[_index+6]*3+2]};

        Vec3 p2_transformed_to_p1 = p2-p1;
        Vec3 p3_transformed_to_p1 = p3-p1;

        Vec3 point_direction = ray.Direction.cross(p3_transformed_to_p1);
        Prec determinante = p2_transformed_to_p1.dot(point_direction);

        //check backfacing or never intersection
        if(determinante < epsilon)
            return std::nullopt;

        Prec inv_determinante = 1 / determinante;


        Vec3 ray_origin_transformed_to_p1 = ray.Origin - p1;

        //UV
        Prec u = ray_origin_transformed_to_p1.dot(point_direction) * inv_determinante;
        if(u < ERROR_THICCNESS || u > 1 - ERROR_THICCNESS)
            return std::nullopt;

        Vec3 cross_base_transformed_ray_p1p2 = ray_origin_transformed_to_p1.cross(p2_transformed_to_p1);
        Prec v = ray.Direction.dot(cross_base_transformed_ray_p1p2) * inv_determinante;
        if(v < ERROR_THICCNESS || u+v > 1 - ERROR_THICCNESS)
            return std::nullopt;
        
        //intersection point
        Prec intersection_point_distance = p3_transformed_to_p1.dot(cross_base_transformed_ray_p1p2) * inv_determinante;

        if(intersection_point_distance < 0)
            return std::nullopt;

        //output
        IntersectionData output;
        output.Distance = intersection_point_distance;
        output.Shape = (IShape*)this;
        output.UV = {u,v};
        
        return std::make_optional<IntersectionData>(output);
    }

    const SurfaceProperties Triangle::GetSurfaceProperties(const Ray& ray,const IntersectionData& data) const 
    {
        SurfaceInteraction interaction;
        
        interaction.Point = ray.Direction * data.Distance + ray.Origin ;
        interaction.Distance = data.Distance;
        Vec2 uv1 = {(*_mesh->UVs)[(*_mesh->Indices)[_index+1]*2], (*_mesh->UVs)[(*_mesh->Indices)[_index+1]*2+1]};
        Vec2 uv2 = {(*_mesh->UVs)[(*_mesh->Indices)[_index+4]*2], (*_mesh->UVs)[(*_mesh->Indices)[_index+4]*2+1]};
        Vec2 uv3 = {(*_mesh->UVs)[(*_mesh->Indices)[_index+7]*2], (*_mesh->UVs)[(*_mesh->Indices)[_index+7]*2+1]};

        
        Vec3 n1 = {(*_mesh->Normals)[(*_mesh->Indices)[_index+2]*3], (*_mesh->Normals)[(*_mesh->Indices)[_index+2]*3+1], (*_mesh->Normals)[(*_mesh->Indices)[_index+2]*3+2]};
        Vec3 n2 = {(*_mesh->Normals)[(*_mesh->Indices)[_index+5]*3], (*_mesh->Normals)[(*_mesh->Indices)[_index+5]*3+1], (*_mesh->Normals)[(*_mesh->Indices)[_index+5]*3+2]};
        Vec3 n3 = {(*_mesh->Normals)[(*_mesh->Indices)[_index+8]*3], (*_mesh->Normals)[(*_mesh->Indices)[_index+8]*3+1], (*_mesh->Normals)[(*_mesh->Indices)[_index+8]*3+2]};
        
        interaction.UV = LinAlg::Interpolate(uv1, uv2, uv3, data.UV[0], data.UV[1]);
        interaction.Normal = LinAlg::Interpolate(n1, n2, n3, data.UV[0], data.UV[1]);

        //interaction.Normal = n1;

        return SurfaceProperties(interaction,_mesh->Material.get());
    }
    
    Bounds3D<Prec> Triangle::WorldBoundary() const
    {
        Prec p1_x = (*_mesh->Vertices)[(*_mesh->Indices)[_index]*3];
        Prec p1_y = (*_mesh->Vertices)[(*_mesh->Indices)[_index]*3 + 1];
        Prec p1_z = (*_mesh->Vertices)[(*_mesh->Indices)[_index]*3 + 2];

        Prec p2_x = (*_mesh->Vertices)[(*_mesh->Indices)[_index + 3]*3];
        Prec p2_y = (*_mesh->Vertices)[(*_mesh->Indices)[_index + 3]*3 + 1];
        Prec p2_z = (*_mesh->Vertices)[(*_mesh->Indices)[_index + 3]*3 + 2];

        Prec p3_x = (*_mesh->Vertices)[(*_mesh->Indices)[_index + 6]*3];
        Prec p3_y = (*_mesh->Vertices)[(*_mesh->Indices)[_index + 6]*3 + 1];
        Prec p3_z = (*_mesh->Vertices)[(*_mesh->Indices)[_index + 6]*3 + 2];

        return Bounds3D<Prec>(std::max({p1_x, p2_x, p3_x}),
                                        std::min({p1_x, p2_x, p3_x}),
                                        std::max({p1_y, p2_y, p3_y}),
                                        std::min({p1_y, p2_y, p3_y}),
                                        std::max({p1_z, p2_z, p3_z}),
                                        std::min({p1_z, p2_z, p3_z}));

        return Bounds3D<Prec>();
    }

}