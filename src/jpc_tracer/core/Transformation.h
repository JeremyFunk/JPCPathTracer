#pragma once
#include"Base.h"
#include "Linalg.h"
#include "core/Transformation.h"
#include<memory.h>

namespace jpc_tracer
{
    
    class Transformation
    {
    public:
        Vec3 Position, Rotation, Scale;
        Mat4x4 L2W;
        Mat3x3 Rot2W ;

        Transformation(Vec3 position, Vec3 rotation, Vec3 scale);
        Transformation(Vec3 position);
        Transformation() = default;
    private:
        void RecalculateMatrix();
    };
    
}