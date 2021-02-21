#pragma once

#include "core/Bounds3D.h"

namespace jpc_tracer
{
    struct BVHNode
    {
        void InitLeaf(Bounds3D<Prec>& bounds, const int& number_shapes, const int& idx_shape_start, const int& idx_shape_end);
        
        void InitInner(Bounds3D<Prec>& bounds, const int& number_shapes, BVHNode* first_child, BVHNode*  second_child);

        Bounds3D<Prec> Bounds;
        int Number_Shapes = -1;
        int Idx_Shape_Start = -1;
        int Idx_Shape_End = -1;
        BVHNode* Idx_First_Child = nullptr;
        BVHNode* Idx_Second_Child = nullptr;
    };

    struct SmallBVHNode
    {
        SmallBVHNode(Bounds3D<Prec> Bounds, int Number_Shapes, int Idx_Shape_Start, int Idx_Second_Child);
        SmallBVHNode(Bounds3D<Prec> Bounds, int Number_Shapes, int Idx_Shape_Start);

        Bounds3D<Prec> Bounds;
        int Number_Shapes;
        int Idx_Shape_Start;
        int Idx_Second_Child;
    };

    struct ShapeInfo
    {
        ShapeInfo() = default;
        ShapeInfo(const size_t& shape_idx, const Bounds3D<Prec>& bounds);

        size_t Shape_Idx;
        Bounds3D<Prec> Bounds;
        Vec3 Center;
    };
}