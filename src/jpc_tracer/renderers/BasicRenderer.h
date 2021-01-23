#pragma once
#include "core/Bsdf.h"
#include "core/IRenderer.h"
#include "core/IFilm.h"

namespace jpc_tracer
{
 

    class BasicRenderer : public IRenderer
    {
    public:
        BasicRenderer(int sample_count, bool multithread = true);
        virtual void Init(Ref<ISampler> sampler, Ref<ICamera> camera, Ref<IScene> scene,
         Ref<IIntegrator> integrator, Ref<IFilter> filter ,Ref<IFilm> film) override;
        virtual void Render() override;
        virtual void Finish() override;
        void EvaluateTile(int x, int y, int w, int h);
        
    private:
        Ref<ISampler> _sampler; 
        Ref<ICamera> _camera; 
        Ref<IScene> _scene;
        Ref<IIntegrator> _integrator;
        Ref<IFilm> _film;
        Ref<IFilter> _filter;
        int _sample_count_x;
        int _sample_count_y;
        bool _multithread;
        BsdfMemoryInfo _bsdf_info;
    };
}