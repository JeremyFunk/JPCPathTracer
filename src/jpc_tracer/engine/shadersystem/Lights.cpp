#include "Lights.h"
#include "jpc_tracer/engine/shadersystem/BsdfNode.h"
#include "jpc_tracer/engine/shadersystem/NormalSpace.h"
namespace jpctracer::shadersys
{
    void Lights::AddPointLight(const Vec3& position,Spectrum color) 
    {
        point_lights.Add(position, color);        
    }
    
    const ShaderResults Lights::Sample(View<Vec2> samples,const SurfaceInteraction& interaction) const
    {
        ShaderResults results(0,samples.size);
        point_lights.Sample(results.sampled_results, results.sampled_rays, samples,interaction);
        return results;
    
    }
}