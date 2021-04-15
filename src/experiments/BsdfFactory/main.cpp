
#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/engine/PluginsApi.h"
#include "jpc_tracer/plugins/shaders/DebugBsdf.h"


#include <cmath>
#include <iostream>

namespace jpctracer {
    inline BsdfNode DebugBsdf1(ShaderContext* ctx, Spectrum spec)
    {
        return CreateBsdf<MATERIAL_DIFFUSE, DebugBsdfClosure>(ctx,spec);
    }
    inline BsdfNode DebugBsdf2(ShaderContext* ctx, Spectrum spec)
    {
        return CreateBsdf<MATERIAL_GLOSSY, DebugBsdfClosure>(ctx,spec);
    }

    inline BsdfNode material(ShaderContext* ctx,Prec val)
    {
        Prec val_glossy = std::sqrt(val);
        DebugBsdf1(ctx, FromRGB({val,val,val}));
        return DebugBsdf2(ctx, FromRGB({val_glossy,val_glossy,val_glossy}));


    }

    ShaderResults shader(View<Ray> rays, View<Vec2> samples,float val)
    {

        return SampleShader<MATERIAL_GLOSSY>(ShaderBind(material,val),NormalSpace(),
        rays,samples);
    }

}

int main(int argc,char *argv[])
{
    std::cout<<"test\n";
    int i;
    jpctracer::Ray rays[5];
    jpctracer::Vec2 samples[5];
    
    jpctracer::ShaderResults results = jpctracer::shader({rays,4},{samples,4},(float)argc);

    
    return 0;
}
