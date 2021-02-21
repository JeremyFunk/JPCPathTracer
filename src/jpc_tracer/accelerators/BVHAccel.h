#pragma once

#include "BVHHelperStructs.h"
#include "core/Bounds3D.h"
#include "core/IShape.h"
#include "core/Linalg.h"
#include <memory>
#include <vector>

namespace jpc_tracer
{
    class BVHAccel
    {
    public:
        BVHAccel(Ref<std::vector<Ref<IShape>>> shapes, const int& max_shapes_in_leaf);

        std::optional<IntersectionData> Traversal(const Ray& ray) const;

    private:
        void BuildBVH();

        void RecursiveBuild(int start, int end, int& offset);

        void split_equal_subsets(const int& dim, const int& start, const int& mid, const int& end);

        const int _max_shapes_in_leaf;
        Ref<std::vector<Ref<IShape>>> _shapes;
        std::vector<ShapeInfo> _shapes_info;
        
        // linear tree
        // traversal: node, left, right
        std::vector<SmallBVHNode> _tree;
    };
}