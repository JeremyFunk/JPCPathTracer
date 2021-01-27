#include "BVHHelperStructs.h"

namespace jpc_tracer
{
    void BVHNode::InitLeaf(Bounds3D<Prec>& bounds, const int& number_shapes, const int& idx_shape_start, const int& idx_shape_end)
        {
            Bounds = bounds;
            Number_Shapes = number_shapes;
            Idx_Shape_Start = idx_shape_start;
            Idx_Shape_End = idx_shape_end;
        }

    void BVHNode::InitInner(Bounds3D<Prec>& bounds, const int& number_shapes, BVHNode* first_child, BVHNode*  second_child) 
    {
        Bounds = bounds;
        Number_Shapes = number_shapes;
        Idx_First_Child = first_child;
        Idx_Second_Child = second_child;
    }
    
    SmallBVHNode::SmallBVHNode(Bounds3D<Prec> bounds, int number_shapes, int idx_shape_start, int idx_second_child) 
        : Bounds(bounds), Number_Shapes(number_shapes), Idx_Shape_Start(idx_shape_start), Idx_Second_Child(idx_second_child)
    {
        
    }
    
    SmallBVHNode::SmallBVHNode(Bounds3D<Prec> bounds, int number_shapes, int idx_shape_start) 
        : Bounds(bounds), Number_Shapes(number_shapes), Idx_Shape_Start(idx_shape_start), Idx_Second_Child(0)
    {
        
    }

    ShapeInfo::ShapeInfo(const size_t& shape_idx, const Bounds3D<Prec>& bounds) 
        : Shape_Idx(shape_idx), Bounds(bounds), 
        Center(0.5f * (bounds.Max.x() +bounds.Min.x()), 0.5f * (bounds.Max.y() +bounds.Min.y()), 0.5f * (bounds.Max.z() +bounds.Min.z()))
    {

    }
}