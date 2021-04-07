#pragma once
#include "Base.h"
#include "SceneBuilder.h"


namespace jpctracer::raytracing {

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



    UserThreadData* Clone(UserThreadData* data,ComputationCell cell);
    void Free(UserThreadData* data);



    struct PrivateContext;

    struct Context
    {
        void SetComputationGrid(ComputationGrid grid);
        void SetRayGenator(void (*ray_generator)(int x, int y, UserThreadData* data, const Scene* scene));  
        void SetThreadDataSaver(void (*thread_saver)(UserThreadData* datas, int count));      
        void SetScene();
        PrivateContext* data;
    };
    
    

    void LaunchTracingEngine(const Context* context, const Scene* scene);
}