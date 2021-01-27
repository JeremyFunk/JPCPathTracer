#pragma once
#include"Base.h"
#include "ISampler.h"
#include <memory>
#include "ICamera.h"
#include "IScene.h"
#include "IIntegrator.h"
#include "IFilter.h"
#include "core/IFilm.h"

namespace jpc_tracer {
    class IRenderer
    {
    public:

        virtual void Init(Ref<ISampler> sampler, Ref<ICamera> camera, 
        Ref<IScene> scene, Ref<IIntegrator> integrator, Ref<IFilter> filter, 
        Ref<IFilm> film ) = 0;
        virtual void Render() = 0;
        virtual void Finish() = 0;
        virtual void WriteImage(std::string path) const = 0;
        virtual ~IRenderer(){}
    };
}