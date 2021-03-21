#pragma once

#include "ProjectionCamera.h"
    
namespace jpctracer::camera {
    template<class T>
    auto Build(T builder) {return builder.Build();}
}


