#pragma once
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/engine/renderer/Tracer.h"
#include "jpc_tracer/engine/films/Film.h"
#include <memory>

namespace jpctracer::renderer {
    struct ICamera
    {
        virtual void SetWidthHeight(uint width, uint height) = 0;
        virtual Ray Generate(UInt2 pixel,Vec2 random_point) const = 0;
        virtual ~ICamera() = 0;
    };
    inline ICamera::~ICamera()
    {}

    struct ISampler
    {
        virtual void Sample(UInt2 count, Vec2* destination) = 0;
        virtual void Sample(UInt3 count, Vec3* destination) = 0;
        virtual ISampler* Clone() const = 0;
        virtual ~ISampler() = 0;
    };
    inline ISampler::~ISampler()
    {}

    struct IIntegrator
    {
        virtual void Integrate(UInt2 pixel, const ICamera* camera, ISampler* sampler,
            Tracer& tracer, film::Film& film) const = 0;
        virtual ~IIntegrator() = 0;
    };
    inline IIntegrator::~IIntegrator()
    {}
}