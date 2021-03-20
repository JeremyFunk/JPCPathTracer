#include "Concepts.h"


namespace jpctracer::shader
{
    /*
    template<>
    Spectrum Setup(Spectrum color,plugins::ShaderCache& chache) 
    {
        return color;
    }*/
    
    Prec Setup(Prec value,plugins::ShaderCache* chache) 
    {
        return value;
    }
    
    Vec3 Setup(Vec3 normal,plugins::ShaderCache* chache) 
    {
        return normal;
    }

    namespace shader
    {
        Spectrum EvalColor(Spectrum c) 
        {
            return c;
        }
        
        Prec EvalValue(Prec v) 
        {
            return v;
        }
        
        Vec3 EvalNormal(Vec3 n) 
        {
            return n;
        }
    }
}