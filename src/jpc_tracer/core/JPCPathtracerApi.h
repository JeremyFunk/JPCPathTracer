#pragma once

#include "IScene.h"
#include "ICamera.h"
#include "ISampler.h"
#include "IIntegrator.h"
#include "IFilter.h"
#include "IFilm.h"
#include "IRenderer.h"
#include "constants.h"
#include "core/Base.h"

namespace jpc_tracer
{
    class JPCPathtracerApi
    {
    public:
        JPCPathtracerApi();
        virtual Scope<IScene> GetScene() = 0;
        virtual Scope<ICamera> GetCamera() = 0;
        virtual Scope<ISampler> GetSampler() = 0;
        virtual Scope<IIntegrator> GetIntegrator() = 0;
        virtual Scope<IFilter> GetFiler() = 0;
        virtual Scope<IFilm> GetFilm() = 0;
        virtual Scope<IRenderer> GetRenderer() = 0;
        virtual ~JPCPathtracerApi() = 0;
        virtual Scope<IRenderer> FullRenderEngine() = 0;
        bool IsAlreadyStarted() const;
        
    private:
        static std::atomic<bool> _already_started;
    };


}