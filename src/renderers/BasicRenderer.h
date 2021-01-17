#pragma once
#include "core/Bsdf.h"
#include "core/IRenderer.h"
#include "core/IFilm.h"

namespace renderers
{
    using namespace core;

    class BasicRenderer : public IRenderer
    {
    public:
        BasicRenderer(int sample_count, bool multithread = true);
        virtual void Init(std::shared_ptr<ISampler> sampler, std::shared_ptr<ICamera> camera, std::shared_ptr<IScene> scene,
         std::shared_ptr<IIntegrator> integrator, std::shared_ptr<IFilter> filter ,std::shared_ptr<IFilm> film) override;
        virtual void Render() override;
        virtual void Finish() override;
        void EvaluateTile(int x, int y, int w, int h);
        
    private:
        std::shared_ptr<ISampler> _sampler; 
        std::shared_ptr<ICamera> _camera; 
        std::shared_ptr<IScene> _scene;
        std::shared_ptr<IIntegrator> _integrator;
        std::shared_ptr<IFilm> _film;
        std::shared_ptr<IFilter> _filter;
        int _sample_count_x;
        int _sample_count_y;
        bool _multithread;
        BsdfMemoryInfo _bsdf_info;
    };
}