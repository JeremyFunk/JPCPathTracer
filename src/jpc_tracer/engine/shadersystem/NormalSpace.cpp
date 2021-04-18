#include "NormalSpace.h"
#include "jpc_tracer/core/maths/Constants.h"
namespace jpctracer::shadersys
{
    Vec3 WorldToNormal(const Vec3& x, const NormalSpace& mem) 
    {
       return Vec3({mem.Tangent1.dot(x),mem.Tangent2.dot(x),mem.Normal.dot(x)});    
    }
    
    Norm3 WorldToNormal(const Norm3& x, const NormalSpace& mem) 
    {
        return Norm3::UnsafeConversion(WorldToNormal(Vec3(x),mem));
    }

    Ray WorldToNormal(const Ray& x, const NormalSpace& mem) 
    {
        Norm3 dir = WorldToNormal(x.Direction,mem);
        Vec3 origin = mem.Interaction.Point-x.Origin;
        return Ray{dir,origin,x.LensPosition,x.ClipEnd,x.Time};
    }

    Vec3 NormalToWorld(const Vec3& x,const NormalSpace& mem)
    {
        const Norm3& tangent1 = mem.Tangent1; 
        const Norm3& tangent2 = mem.Tangent2; 
        const Norm3& normal = mem.Normal;
        return Vec3({
            tangent1[0]*x[0]+tangent2[0]*x[1]+normal[0]*x[2],
            tangent1[1]*x[0]+tangent2[1]*x[1]+normal[1]*x[2],
            tangent1[2]*x[0]+tangent2[2]*x[1]+normal[2]*x[2]
        });
    }
    
    Norm3 NormalToWorld(const Norm3& x,const NormalSpace& mem) 
    {
        return Norm3::UnsafeConversion(NormalToWorld(Vec3(x),mem));
    }

    Ray NormalToWorld(const Ray& x, const NormalSpace& mem) 
    {
        Norm3 dir = NormalToWorld(x.Direction,mem);
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

        Norm3 scattering_dir = WorldToNormal(scattering_ray.Direction, mem);
        //Flip ray
        scattering_dir.flip();

        mem.ScatteringRay = Ray{scattering_dir,{0,0,0},0,0,scattering_ray.Time};
        
        

        return mem;
    }
}