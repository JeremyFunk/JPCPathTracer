#pragma once

#include "jpc_tracer/core/maths/Transformation.h"
#include <vector>

namespace jpctracer::raytracing
{
    struct BVHNode
    {
        Bounds3D bound;
        
        // first_idx <= split_idx <= last_id
        uint first_idx;
        uint last_idx;
        uint split_idx;
    };

    struct BVHTree
    {
        std::vector<Bounds3D> shape_bounds; // size n
        std::vector<BVHNode> internal_nodes; //size n-1

        BVHTree() = default;

        BVHTree(std::vector<Bounds3D>&& bounds) : shape_bounds(bounds), internal_nodes(bounds.size() - 1)
        {}
    };
}