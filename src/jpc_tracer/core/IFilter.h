#pragma once
#include"Base.h"
#include "Linalg.h"
#include "Bounds2D.h"
#include "core/Bounds2D.h"
namespace jpc_tracer {
    class IFilter
    {
    public:
        IFilter(Bounds2D<int>bounding_box): BoundingBox(bounding_box) {}
        //Center at 0,0
        virtual float Evaluate(Vec2 point) = 0;
        Bounds2D<int> BoundingBox;
        virtual ~IFilter(){}
    };
}