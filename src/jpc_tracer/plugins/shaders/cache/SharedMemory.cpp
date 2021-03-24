#include "SharedMemory.h"
namespace jpctracer::shader::detail
{
    Vec3 WorldToNormal(const Vec3& x, const SharedMemory& mem) 
    {
       return Vec3({x.dot(mem.Tangent1),x.dot(mem.Tangent2),x.dot(mem.Normal)});    
    }
    
    Ray WorldToNormal(const Ray& x, const SharedMemory& mem) 
    {
        Vec3 dir = WorldToNormal(x.Direction,mem);
        Vec3 origin = mem.Interaction.Point-x.Origin;
        return Ray{dir,origin,x.LensPosition,x.ClipEnd,x.Time};
    }

    Vec3 NormalToWorld(const Vec3& x,const SharedMemory& mem)
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
    
    Ray NormalToWorld(const Ray& x, const SharedMemory& mem) 
    {
        Vec3 dir = NormalToWorld(x.Direction,mem);
        Vec3 origin = mem.Interaction.Point+x.Origin;
        return Ray{dir,origin,x.LensPosition,x.ClipEnd,x.Time};
    }
    
    SharedMemory CreateSharedMemory(const Ray& scattering_ray,
                                    const SurfaceInteraction& interaction) 
    {
        SharedMemory mem;
        mem.Interaction = interaction;
        mem.ScatteringRay = scattering_ray;
        mem.Normal = interaction.Normal;

        Vec3 temp_v = {0,0,-1};
        if(mem.Normal[2]!=0)
        {
            temp_v = {0,-1,0};
        }
        mem.Tangent1 = mem.Normal.cross(temp_v).normalized();
        mem.Tangent2 = mem.Normal.cross(mem.Tangent1).normalized();

        mem.ScatteringDir = WorldToNormal(scattering_ray.Direction, mem);

        return mem;
    }
}