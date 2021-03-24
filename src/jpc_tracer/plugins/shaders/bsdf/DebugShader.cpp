#include "DebugShader.h"
#include "jpc_tracer/core/maths/Spectrum.h"



namespace jpctracer::shader::detail
{
    DebugShaderClosure::DebugShaderClosure(Spectrum color) : m_color(color) {}
    
    DebugShaderClosure::DebugShaderClosure() 
        : m_color(Black())
    {
        
    }

    std::pair<Spectrum,Prec> 
    DebugShaderClosure::operator()(const SharedMemory& mem,
                        const Ray& incident_ray) const
    {
        return {m_color,1};

    }

    std::pair<Spectrum,Prec> 
    DebugShaderClosure::operator()(const SharedMemory& mem,
                        Ray* out_incident_ray, Vec2 random_point) const
    {
        return {m_color,1};
    }

}