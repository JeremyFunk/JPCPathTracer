#include "BuildLBVH.h"

#include "jpc_tracer/core/maths/Transformation.h"
#include "jpc_tracer/engine/raytracing/Geometry.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/bvh/BVH.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/bvh/BVHBuilderHelper.h"
#include <stdint.h>
#include <jpc_tracer/engine/raytracing/detail/acceleration/bvh/BVHBuilderHelper.h>
#include <vector>
#include <execution>

namespace jpctracer::raytracing
{
    BVHTree BuildLBVH(std::vector<Bounds3D>&& bounds, std::vector<uint32_t>&& morton_codes)
    {
        BVHTree tree(std::move(bounds));

        const size_t number_nodes = tree.internal_nodes.size();
        const size_t number_shapes = tree.shape_bounds.size();

        uint32_t* morton = morton_codes.data();

        //build binary radix tree
        #pragma omp parallel for
        for(int idx = 0; idx < number_nodes; idx++)
        {
            int direction = calc_direction(morton, idx, number_nodes);

            uint last_idx = calc_last_idx(morton, idx, direction, number_nodes);

            uint min_idx = std::min<uint>(idx, last_idx);
            uint max_idx = std::max<uint>(idx, last_idx);

            uint split_idx = calc_split_idx(morton, min_idx, max_idx, direction, number_nodes);

            const Bounds3D bound = calc_bounds(tree.shape_bounds.data(), min_idx, max_idx);

            tree.internal_nodes[idx] = {bound, min_idx, max_idx, split_idx};
        }

        return tree;
    }
}