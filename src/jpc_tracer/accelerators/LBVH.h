#pragma once

#include "BVHHelperStructs.h"
#include "core/Bounds3D.h"
#include "core/IShape.h"
#include "core/Linalg.h"
#include "core/Ray.h"
#include <memory>
#include <vector>

namespace jpc_tracer
{
    struct ShapeMortonInfo
    {
        size_t Shape_Idx;
        Bounds3D<Prec> Bounds;
        uint32_t morton_code;
    };

    struct LinearTreeNode
    {
        Bounds3D<Prec> Bounds;
        int Min_Idx;
        int Max_Idx;
        int Split_Idx;
    };

    class LBVHAccel
    {
        /*
        * inspired by: https://research.nvidia.com/sites/default/files/pubs/2012-06_Maximizing-Parallelism-in/karras2012hpg_paper.pdf
        */

    public:
        LBVHAccel(Ref<std::vector<Ref<IShape>>> shapes);

        std::optional<IntersectionData> Traversal(const Ray& ray) const;

    private:
        void BuildLBVH();

        void generate_shapes_code(const size_t& shapes_size);
        void sort_shapes_code();

        int calculate_direction(const int& i);
        int upper_range_bound(const int& i, const int& direction);
        int lower_range_bound(const int& i, const int& direction, const int& max_range);
        int calculate_split_index(const int& i, const int& direction, const int& last_idx, const int& min_range);

        inline int length_combined_leading_zeros(const size_t& idx_a, const size_t& idx_b) const;
        inline uint32_t EncodeMorton3(const Vec3& vec) const;
        inline uint32_t LeftShift3(uint32_t x) const;

        void shape_intersection(const int& idx, const Ray& ray, std::optional<IntersectionData>& closestInteraction) const;

        //member variables
        Ref<std::vector<Ref<IShape>>> _shapes;
        std::vector<ShapeMortonInfo> _shapes_code_info;

        std::vector<LinearTreeNode> _tree;
    };
}