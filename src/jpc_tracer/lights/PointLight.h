#pragma once

#include "core/Linalg.h"
#include "core/Ray.h"
#include "core/Spectrum.h"
#include "core/ILight.h"

namespace jpc_tracer {


    class PointLight : public ILight
    {
    public:
        PointLight(const Vec3& position, const Spectrum& spectrum);

        virtual LightInformation GetLightInformation(const Vec3& point) const override;
        virtual Spectrum Illumination(const Vec3& direction, const LightInformation& lightInformation) const override;

    private:
        Vec3 _position;
        Spectrum _spectrum;
    };
}