#pragma once
#include "Spectrum.h"
#include "SurfaceInteraction.h"
#include "Ray.h"
#include "core/SpectrumPasses.h"
namespace core {
    struct DirectionInfo
    {
    public:
        DirectionInfo(SpectrumPasses surface_color,
        float pdf,  Vec3 outgoing_direction) : SurfaceColor(surface_color),Pdf(pdf), OutgoingDirection(outgoing_direction) {}

        DirectionInfo(){}

        SpectrumPasses SurfaceColor;
        Vec3 OutgoingDirection;
        float Pdf = 1;    
    };
    class BxDFType
    {
        bool BSDF_REFLECTION = true; 
        bool BSDF_TRANSMISSION = true;
        bool BSDF_DIFFUSE = true;
        bool BSDF_GLOSSY = true;
        bool BSDF_SPECULAR = true;
    };
    class IBSDF
    {
    public:
        //Outgoing Direction should be an Random Variable on a hemisphere
        virtual DirectionInfo Evaluate(const Vec3& incoming_direction,const Vec2& random_point,BxDFType type = BxDFType()) = 0;

    };
}