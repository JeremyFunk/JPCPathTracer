#include "JPCDefaultApi.h"
#include "cameras/ProjectionCamera.h"
#include "core/Base.h"
#include "core/ICamera.h"
#include "core/IFilm.h"
#include "core/IFilter.h"
#include "core/IIntegrator.h"
#include "core/ISampler.h"
#include "core/IScene.h"
#include "core/JPCPathtracerApi.h"
#include "core/Spectrum.h"
#include "films/BasicFilm.h"
#include "filters/GaussianFilter.h"
#include "integrators/PathIntegrator.h"
#include "materials/BasicMaterial.h"
#include "renderers/BasicRenderer.h"
#include "samplers/StratifiedSampler.h"
#include "scenes/BVHScene.h"
namespace jpc_tracer
{
    JPCDefaultApi::JPCDefaultApi(DefaultSettings& settings) 
        : JPCPathtracerApi(), _settings(settings)
    {
        
    }
    
    Scope<IScene> JPCDefaultApi::GetScene() 
    {
        return MakeScope<BVHScene>(GetShapes(),GetLights());
    }
    
    Scope<ICamera> JPCDefaultApi::GetCamera() 
    {
        return MakeScope<ProjectionCamera>(
            _settings.Width,_settings.Height,
            _settings.NearPlane
            );
    }
    
    Scope<ISampler> JPCDefaultApi::GetSampler() 
    {
        return MakeScope<StratifiedSampler>();
    }
    
    Scope<IIntegrator> JPCDefaultApi::GetIntegrator() 
    {
        return MakeScope<PathIntegrator>(Spectrum::FromRGB(_settings.SkyColor),_settings.MaxRayDepth);
    }
    
    Scope<IFilter> JPCDefaultApi::GetFiler() 
    {
        return MakeScope<GaussianFilter>(_settings.FilmGaussianFilter);
    }
    
    Scope<IFilm> JPCDefaultApi::GetFilm() 
    {
        return MakeScope<BasicFilm>(_settings.Width,_settings.Height);
    }
    
    Scope<IRenderer> JPCDefaultApi::GetRenderer() 
    {
        return MakeScope<BasicRenderer>(_settings.SampleCount,true);
    }
    
    Scope<IRenderer> JPCDefaultApi::FullRenderEngine() 
    {
        auto renderer = GetRenderer();
        Ref<ICamera> camera = GetCamera();
        Ref<ISampler> sampler = GetSampler();
        Ref<IScene> scene = GetScene();
        Ref<IIntegrator> integrator = GetIntegrator();
        integrator->Init(scene);
        Ref<IFilter> filter = GetFiler();
        Ref<IFilm> film = GetFilm();
        renderer->Init(sampler,camera , scene,integrator,filter,film);
        return std::move(renderer);
    }

    JPCDefaultApi::~JPCDefaultApi() 
    {
        
    }
}