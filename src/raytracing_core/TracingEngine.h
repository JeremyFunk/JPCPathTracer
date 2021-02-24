#pragma once

#include "raytracing_core/Scene.h"
namespace jpc_rt_core {

    struct ComputationGrid
    {
        int Width;
        int Height;
        int TileSize;
    };

    struct ComputationCell
    {
        int XMin;
        int XMax;
        int YMin;
        int YMax;
    
    };



    ThreadData* Create(const GlobalData* data,ComputationCell cell);
    void Free(ThreadData* data);



    struct PrivateContext;

    struct Context
    {
        void SetCompitationGrid(ComputationGrid grid);
        void SetMaxStackDepth(int depth = 20);
        void SetRayGenator(void (*ray_generator)(int x, int y, ThreadData* data, const Scene* scene));  
        void SetThreadDataSaver(void (*thread_saver)(GlobalData* data, ThreadData* datas, int count));      
        PrivateContext* data;
    };
    
    

    void LaunchTracingEngine(const Context* context, const Scene* scene);
}