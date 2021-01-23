#pragma once
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

        virtual void Init(std::shared_ptr<ISampler> sampler, std::shared_ptr<ICamera> camera, 
        std::shared_ptr<IScene> scene, std::shared_ptr<IIntegrator> integrator, std::shared_ptr<IFilter> filter, 
        std::shared_ptr<IFilm> film ) = 0;
        virtual void Render() = 0;
        virtual void Finish() = 0;

    };
}