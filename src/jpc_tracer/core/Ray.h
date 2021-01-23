#pragma once
#include"Base.h"
#include "Linalg.h"
namespace jpc_tracer {
    class Ray
    {
    public:
        Ray(Vec3 origin, Vec3 direction) : Origin(origin), Direction(direction){}
        Ray(Vec3 origin, Vec3 direction, int depth) : Origin(origin), Direction(direction), Depth(depth){}
        Ray() : Origin({0,0,0}), Direction({0,0,0}){}

        Vec3 Origin;
        Vec3 Direction;
        Vec2 LensPosition;
        int Depth = 0;
    };
}