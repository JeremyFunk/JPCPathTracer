#pragma once

#include "core/Bounds3D.h"

namespace accel
{
    struct BVHNode
    {
        void InitLeaf(core::Bounds3D<core::Prec>& bounds, const int& number_shapes, const int& idx_shape_start, const int& idx_shape_end);
        
        void InitInner(core::Bounds3D<core::Prec>& bounds, const int& number_shapes, BVHNode* first_child, BVHNode*  second_child);

        core::Bounds3D<core::Prec> Bounds;
        int Number_Shapes = -1;
        int Idx_Shape_Start = -1;
        int Idx_Shape_End = -1;
        BVHNode* Idx_First_Child = nullptr;
        BVHNode* Idx_Second_Child = nullptr;
    };

    struct SmallBVHNode
    {
        core::Bounds3D<core::Prec> Bounds;
        int Number_Shapes;
        int Idx_Shape_Start;
        int Idx_Second_Child;
    };

    struct ShapeInfo
    {
        ShapeInfo() = default;
        ShapeInfo(const size_t& shape_idx, const core::Bounds3D<core::Prec>& bounds);

        size_t Shape_Idx;
        core::Bounds3D<core::Prec> Bounds;
        core::Vec3 Center;
    };
}