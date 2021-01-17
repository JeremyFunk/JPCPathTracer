#include "BasicRenderer.h"
#include "core/Linalg.h"
#include "core/MemoryArea.h"
#include "core/Spectrum.h"
#include "core/SpectrumPasses.h"
#include <algorithm>
#include <memory>
#include <math.h>
#include <atomic>
#include <iostream>
#include <omp.h>
#include <vector>

namespace renderers
{
 
     BasicRenderer::BasicRenderer(int sample_count,bool multithread) 
        : _multithread(multithread)
     {
         _sample_count_x = ceil(sqrt(sample_count));
         _sample_count_y = ceil(sqrt(sample_count));
     }
 

    void BasicRenderer::Init(std::shared_ptr<ISampler> sampler, std::shared_ptr<ICamera> camera, std::shared_ptr<IScene> scene,
         std::shared_ptr<IIntegrator> integrator, std::shared_ptr<IFilter> filter ,std::shared_ptr<IFilm> film)
    {
        _sampler = sampler;
        _camera = camera;
        _scene = scene;
        _integrator = integrator;
        _film = film;
        _filter = filter;
    }


    void BasicRenderer::Render(){
        int width = _film->GetWidth();
        int height = _film->GetHeight();

        int tileSize = 16;
        int totalTilesX = ceil(width / (Prec)tileSize);
        int totalTilesY = ceil(height / (Prec)tileSize);

        std::atomic_int tileFinishCounter = 0;

        std::cout<<"Rendering...\n";

        #pragma omp parallel for
        for (int t = 0; t < totalTilesX * totalTilesY; t++)
        {
            int tX = t % totalTilesX;
            int tY = t / totalTilesX;

            int currentTileX = tX * tileSize;
            int currentTileY = tY * tileSize;
            int currentTileSizeX = tX == totalTilesX - 1 ? width - (tX * tileSize) : tileSize;
            int currentTileSizeY = tY == totalTilesY - 1 ? height - (tY * tileSize) : tileSize;

            EvaluateTile(currentTileX, currentTileY, currentTileSizeX, currentTileSizeY);

            tileFinishCounter += 1;
            if (tileFinishCounter % 30 == 0)
                std::cout << ((Prec)tileFinishCounter / ((Prec)totalTilesX * totalTilesY)) * 100 << "%\n";
        }
   


    }

    void BasicRenderer::EvaluateTile(int tX, int tY, int tW, int tH){
        auto myTile = std::make_unique<FilmTile>(_filter, Bounds2D<int>(tX+tW, tX, tY+tH, tY));
        MemoryArea ThreadMemory;
        auto threadSampler = _sampler->Clone();
        
        int sample_count = _integrator->Get2DSampleCount();
        auto samples = std::make_shared<std::vector<core::Vec2>>(_sample_count_x*_sample_count_y*sample_count);

        for(int x = tX; x < tX+tW; x++){
            for(int y = tY; y < tY+tH; y++){
        // for(int x = 362; x < tX+tW; x++){
        //     for(int y = 158; y < tY+tH; y++){
                //if (x == 24 && y == 23)
                //{
                //    std::cout << "Test" << std::endl;
                //}
                threadSampler->Get2DSampleArray(_sample_count_y, _sample_count_x, sample_count, samples->data());
                
                for(int sample_idx_y = 0; sample_idx_y < _sample_count_y; sample_idx_y++){
                    for(int sample_idx_x = 0; sample_idx_x < _sample_count_x; sample_idx_x++)
                    {
                        core::Vec2* sample_start = &samples->operator[](_sample_count_x*sample_count*sample_idx_y+sample_idx_x*sample_count);

                        auto ray = _camera->GenerateRay(threadSampler, Vec2i(x,y));
                        SpectrumPasses sample_pass = _integrator->Integrate(ray,sample_start,ThreadMemory);
                        Vec3 rgb = sample_pass.GetCombined().ToRGB();
                        
                        myTile->AddSample(sample_pass, ray.LensPosition);
                    }
                }
            }   
        }
        

        _film->SaveFilmTile(*myTile);
    }


    void BasicRenderer::Finish(){
        
    }
}
