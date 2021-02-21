#pragma once
#include "core/Base.h"
#include "core/ILight.h"
#include "core/IShape.h"
#include "core/JPCPathtracerApi.h"
#include "core/Linalg.h"
#include <vector>

namespace jpc_tracer {

    struct DefaultSettings
    {
        Prec Width = 500;
        Prec Height = 300;
        int SampleCount = 64;
        Prec NearPlane = 1;
        Prec FilmGaussianFilter = 0.5;
        Vec3 SkyColor = {0,0,0};
        int MaxRayDepth = 2;

        
    };
    
    class JPCDefaultApi : public JPCPathtracerApi
    {
    public:
        JPCDefaultApi(DefaultSettings& settings);
        virtual Ref<std::vector<Ref<IShape>>> GetShapes() = 0;
        virtual Ref<std::vector<Ref<ILight>>> GetLights() = 0;
        virtual Scope<IScene> GetScene();
        virtual Scope<ICamera> GetCamera();
        virtual Scope<ISampler> GetSampler();
        virtual Scope<IIntegrator> GetIntegrator();
        virtual Scope<IFilter> GetFiler();
        virtual Scope<IFilm> GetFilm();
        virtual Scope<IRenderer> GetRenderer();
        virtual Scope<IRenderer> FullRenderEngine();
        ~JPCDefaultApi();
        const DefaultSettings _settings;
    
        
    };
}