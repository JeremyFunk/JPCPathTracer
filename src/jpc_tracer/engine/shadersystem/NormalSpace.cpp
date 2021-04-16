#include "NormalSpace.h"
namespace jpctracer::shadersys
{
    Vec3 WorldToNormal(const Vec3& x, const NormalSpace& mem) 
    {
       return Vec3({x.dot(mem.Tangent1),x.dot(mem.Tangent2),x.dot(mem.Normal)});    
    }
    
    Ray WorldToNormal(const Ray& x, const NormalSpace& mem) 
    {
        Vec3 dir = WorldToNormal(x.Direction,mem);
        Vec3 origin = mem.Interaction.Point-x.Origin;
        return Ray{dir,origin,x.LensPosition,x.ClipEnd,x.Time};
    }

    Vec3 NormalToWorld(const Vec3& x,const NormalSpace& mem)
    {
        const Vec3& tangent1 = mem.Tangent1; 
        const Vec3& tangent2 = mem.Tangent2; 
        const Vec3& normal = mem.Normal;
        return Vec3({
            tangent1[0]*x[0]+tangent2[0]*x[1]+normal[0]*x[2],
            tangent1[1]*x[0]+tangent2[1]*x[1]+normal[1]*x[2],
            tangent1[2]*x[0]+tangent2[2]*x[1]+normal[2]*x[2]
        });
    }
    
    Ray NormalToWorld(const Ray& x, const NormalSpace& mem) 
    {
        Vec3 dir = NormalToWorld(x.Direction,mem);
        Vec3 origin = mem.Interaction.Point+x.Origin;
        return Ray{dir,origin,x.LensPosition,x.ClipEnd,x.Time};
    }
    
    NormalSpace CreateNormalSpace(const Ray& scattering_ray,
                                    const SurfaceInteraction& interaction) 
    {
        NormalSpace mem;
        mem.Interaction = interaction;
        mem.Normal = interaction.Normal;

        Vec3 temp_v = {0,0,-1};
        if(mem.Normal[2]!=0)
        {
            temp_v = {0,-1,0};
        }
        mem.Tangent1 = mem.Normal.cross(temp_v).normalized();
        mem.Tangent2 = mem.Normal.cross(mem.Tangent1).normalized();

        Vec3 scattering_dir = WorldToNormal(scattering_ray.Direction, mem);
        //Flip ray
        scattering_dir*=-1;

        mem.ScatteringRay = Ray{scattering_dir,{0,0,0},0,0,scattering_ray.Time};

        return mem;
    }
}