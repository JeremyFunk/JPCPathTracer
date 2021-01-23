#include "gtest/gtest.h"
#include <iostream>
#include "iostream"
#include "samplers/StratifiedSampler.h"
#include "samplers/DebugSampler.h"
#include "cameras/ProjectionCamera.h"
#include "utilities/printerHelper.h"

namespace jpc_tracer {

    TEST(camera_test,ProjectionCameraRayTest3x3)
    {
        auto sampler = std::make_shared<DebugSampler>();
        auto camera = std::make_shared<ProjectionCamera>(3,3,1);

        for(int x = 0; x < 3; x++){
            for(int y = 0; y < 3; y++){
                auto ray = camera->GenerateRay(sampler, {x,y});
                PrintRay(ray);
            }    
            std::cout<<"\n";
        }
    }
    TEST(camera_test,ProjectionCameraRayTest3x4)
    {
        auto sampler = std::make_shared<DebugSampler>();
        auto camera = std::make_shared<ProjectionCamera>(3,4,1);

        for(int x = 0; x < 3; x++){
            for(int y = 0; y < 4; y++){
                auto ray = camera->GenerateRay(sampler, {x,y});
                PrintRay(ray);
            }    
            std::cout<<"\n";
        }

    }
    TEST(camera_test,ProjectionCameraRayTest4x3)
    {
        auto sampler = std::make_shared<DebugSampler>();
        auto camera = std::make_shared<ProjectionCamera>(4,3,1);

        for(int x = 0; x < 4; x++){
            for(int y = 0; y < 3; y++){
                auto ray = camera->GenerateRay(sampler, {x,y});
                PrintRay(ray);
            }    
            std::cout<<"\n";
        }
    }
}
