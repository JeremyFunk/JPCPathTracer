#include "BVHHelperStructs.h"

namespace accel
{
    void BVHNode::InitLeaf(core::Bounds3D<core::Prec>& bounds, const int& number_shapes, const int& idx_shape_start, const int& idx_shape_end)
        {
            Bounds = bounds;
            Number_Shapes = number_shapes;
            Idx_Shape_Start = idx_shape_start;
            Idx_Shape_End = idx_shape_end;
        }

    void BVHNode::InitInner(core::Bounds3D<core::Prec>& bounds, const int& number_shapes, BVHNode* first_child, BVHNode*  second_child) 
    {
        Bounds = bounds;
        Number_Shapes = number_shapes;
        Idx_First_Child = first_child;
        Idx_Second_Child = second_child;
    }

    ShapeInfo::ShapeInfo(const size_t& shape_idx, const core::Bounds3D<core::Prec>& bounds) 
        : Shape_Idx(shape_idx), Bounds(bounds), 
        Center(0.5f * (bounds.Max.x() +bounds.Min.x()), 0.5f * (bounds.Max.y() +bounds.Min.y()), 0.5f * (bounds.Max.z() +bounds.Min.z()))
    {

    }
}