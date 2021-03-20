#pragma once
#include "Constants.h"

namespace jpctracer
{
    struct Vertex{
        Vec3 Position;
        Vec3 Normal;
        Vec2 UV;

    };
    struct Triangle
    {
        Vertex Vert1;
        Vertex Vert2;
        Vertex Vert3;
    };

    struct Sphere
    {
        Vec3 Position;
        Prec Radius;
    };

}