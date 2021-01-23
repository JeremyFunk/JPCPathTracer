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
    
        void BuildBVH();

    private:
        BVHNode* RecursiveBuild(int start, int end, Ref<std::vector<BVHNode>>& allnodes, int& offset);

        void split_equal_subsets(const int& dim, const int& start, const int& mid, const int& end);

        int linearise_tree(BVHNode* node, Ref<std::vector<SmallBVHNode>>& flattend);

        const int _max_shapes_in_leaf;
        Ref<std::vector<Ref<IShape>>> _shapes;
        std::vector<ShapeInfo> _shapes_info;
        Ref<std::vector<SmallBVHNode>> _tree;
    };
}