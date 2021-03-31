#pragma once
#include "IBsdfClosure.h"
#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "RootShader.h"

namespace jpctracer
{

    struct DebugBsdfClosure final: public IBsdfClosure
    {

        DebugBsdfClosure(Spectrum color = Black());
        virtual std::pair<Spectrum,Prec> operator()(const NormalSpace& context, 
                                Ray incident_ray) const;
        
    
        Spectrum m_color;
    };

    constexpr BsdfNode* DebugBsdf(ShaderContext context, Spectrum color)
    {
        return CreateBsdf<MATERIAL_DIFFUSE |MATERIAL_GLOSSY, DebugBsdfClosure>(context,color);
    }





}