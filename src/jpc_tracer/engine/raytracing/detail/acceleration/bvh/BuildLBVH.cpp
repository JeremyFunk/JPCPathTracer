#include "BuildLBVH.h"

#include "jpc_tracer/core/maths/Transformation.h"
#include "jpc_tracer/engine/raytracing/Geometry.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/bvh/BVH.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/bvh/MortonCode.h"
#include <stdint.h>
#include <vector>
#include <execution>

namespace jpctracer::raytracing
{
    int number_leading_zeros(uint32_t* morton, const int& idx, const int offset, const size_t& number_nodes);
    int calc_direction(uint32_t* morton, const int& idx, const size_t& number_nodes);
    uint calc_last_idx(uint32_t* morton, const int& idx, const int& dir, const size_t& number_nodes);
    uint calc_split_idx(uint32_t* morton, const int& min_idx, const int& max_idx, const int& dir, const size_t& number_nodes);
    Bounds3D calc_bounds(Bounds3D* bound_begin, const Bounds3D* bound_end);

    

    BVHTree BuildLBVH(std::vector<Bounds3D>&& bounds, std::vector<uint32_t>&& morton_codes)
    {
        BVHTree tree(std::move(bounds));

        const size_t number_nodes = tree.internal_nodes.size();
        const size_t number_shapes = tree.shape_bounds.size();

        uint32_t* morton = morton_codes.data();

        //build binary radix tree
        // #pragma omp parallel for
        for(uint idx = 0; idx < number_nodes; idx++)
        {
            int direction = calc_direction(morton, idx, number_nodes);

            uint last_idx = calc_last_idx(morton, idx, direction, number_nodes);

            uint min_idx = std::min(idx, last_idx);
            uint max_idx = std::max(idx, last_idx);

            uint split_idx = calc_split_idx(morton, min_idx, max_idx, direction, number_nodes);

            const Bounds3D bound = calc_bounds(&tree.shape_bounds[min_idx], &tree.shape_bounds[max_idx]);

            tree.internal_nodes[idx] = {bound, min_idx, max_idx, split_idx};
        }

        return tree;
    }

    int number_leading_zeros(uint32_t* morton, const int& idx, const int offset, const size_t& number_nodes)
    {
        const int second_idx = idx + offset;

        if(second_idx < 0 || second_idx > number_nodes) return -1;

        uint32_t morton_a = *(morton + idx);
        uint32_t morton_b = *(morton + second_idx);

        if (morton_a == morton_b)
        {
            size_t temp_idx_a = idx;
            size_t temp_idx_b = second_idx;

            while (temp_idx_a)
            {
                morton_a *= 10;
                temp_idx_a /= 10;
            }

            while (temp_idx_b)
            {
                morton_b *= 10;
                temp_idx_b /= 10;
            }

            morton_a += idx;
            morton_b += second_idx;
        }

        return CountLeadingZerosCombined(morton_a, morton_b);
    }

    int calc_direction(uint32_t* morton, const int& idx, const size_t& number_nodes)
    {
        return (number_leading_zeros(morton, idx, 1, number_nodes) - number_leading_zeros(morton, idx, -1, number_nodes) < 0) ? -1 : 1;
    }

    uint calc_last_idx(uint32_t* morton, const int& idx, const int& dir, const size_t& number_nodes)
    {
        int min_diff = number_leading_zeros(morton, idx, -dir, number_nodes);

        int upper_range = 2;
        while (number_leading_zeros(morton, idx, dir * upper_range, number_nodes) > min_diff)
            upper_range *= 2;

        int lower_range = 0;
        for (int i = upper_range/2; i >= 1; i /= 2)
        {
            if (number_leading_zeros(morton, idx, (lower_range + i) * dir, number_nodes) > min_diff)
                lower_range += i;
        }

        return idx + (lower_range*dir);
    }

    uint calc_split_idx(uint32_t* morton, const int& min_idx, const int& max_idx, const int& dir, const size_t& number_nodes)
    {
        const uint32_t morton_first = *(morton + min_idx);
        const uint32_t morton_last = *(morton + max_idx);

        if(morton_first == morton_last)
            return (min_idx + max_idx) >> 1;

        //int compare_number = __lzcnt(morton_first ^ morton_last); // MSVC
        int compare_number = __builtin_clz(morton_first ^ morton_last);

        int split_idx = 0;
        int step = max_idx - min_idx;

        do
        {
            step = (step + 1) >> 1;
            int new_split = split_idx + step;

            if(new_split < max_idx && number_leading_zeros(morton, min_idx, new_split, number_nodes) > compare_number)
            {
                split_idx = new_split;
            }

        } while(step > 1);

        return split_idx;
    }

    void Union(Bounds3D& a, const Bounds3D& b)
    {
        // a = a & b
        a.Max = {std::max(a.Max[0], b.Max[0]),
                 std::max(a.Max[1], b.Max[1]),
                 std::max(a.Max[2], b.Max[2])};

        a.Min = {std::min(a.Min[0], b.Min[0]),
                 std::min(a.Min[1], b.Min[1]),
                 std::min(a.Min[2], b.Min[2])};
    }

    Bounds3D calc_bounds(Bounds3D* bound_begin, const Bounds3D* bound_end)
    {
        Bounds3D bound = *bound_begin;

        while(++bound_begin != bound_end)
            Union(bound, *bound_begin);

        return bound;
    }

    // struct MortonCodeNode
    // {
    //     uint32_t morton_code;
    //     int idx_shape;
    // };

    // void sort_morton_code(std::vector<MortonCodeNode>::iterator begin, std::vector<MortonCodeNode>::iterator end);
    // void build_binary_radix_tree(BVHNode* nodes, BVHNode* shapes, MortonCodeNode* morton_code, const size_t number_nodes);
    // int number_leading_zeros(MortonCodeNode* morton_code, const int a, const int b);
    // int upper_range_bound(MortonCodeNode* morton_code, const int& first_idx, const int& dir);
    // int lower_range_bound(MortonCodeNode* morton_code, const int& first_idx, const int& dir, const int& max_range);
    // int calculate_split_index(MortonCodeNode* morton_code, const int& first_idx, const int& dir, const int& last_idx);
    // Bounds3D calculate_bounds(BVHNode* shapes, MortonCodeNode* morton_code, const int& first_idx, const int& last_idx);

    // void build_binary_radix_tree(BVHNode* nodes, BVHNode* shapes, MortonCodeNode* morton_code, const size_t number_nodes)
    // {
    //     #pragma omp parallel for
    //     for(int first_idx = 0; first_idx < number_nodes; first_idx++)
    //     {
    //         //direction
    //         int direction = calculate_direction(morton_code, first_idx);

    //         //upper range bound
    //         int max_range = upper_range_bound(morton_code, first_idx, direction);

    //         //lower range bound
    //         int min_range = lower_range_bound(morton_code, first_idx, direction, max_range);

    //         int last_idx = first_idx + min_range*direction;

    //         int min_idx= std::min(first_idx, last_idx);
    //         int max_idx= std::max(first_idx, last_idx);

    //         int split_index = calculate_split_index(morton_code, min_idx, direction, max_idx);

    //         BVHNode* left_child;
    //         BVHNode* right_child;

    //         //left child
    //         if (min_idx == split_index) left_child = shapes + morton_code[split_index].idx_shape; //shape
    //         else left_child = nodes + split_index; //node

    //         //right child
    //         if (max_idx == split_index + 1) left_child = shapes + morton_code[split_index+1].idx_shape; //shape
    //         else left_child = nodes + split_index + 1; //node

    //         nodes[first_idx] = {calculate_bounds(shapes, morton_code, first_idx, last_idx), {left_child, right_child}, false, nullptr, 0};
    //     }
    // }

    // void sort_morton_code(std::vector<MortonCodeNode>::iterator begin, std::vector<MortonCodeNode>::iterator end)
    // {
    //     const auto sort_by_morton = [](const MortonCodeNode& a, const MortonCodeNode& b) { return (a.morton_code < b.morton_code); };

    //     std::sort(std::execution::par, begin, end, sort_by_morton);
    // }

    
    
    // int number_leading_zeros(MortonCodeNode* morton_code, const int a, const int b)
    // {
        // if(b < 0) return -1;

        // uint32_t morton_a = morton_code[a].morton_code;
        // uint32_t morton_b = morton_code[b].morton_code;

        // if (morton_a == morton_b)
        // {
        //     size_t temp_idx_a = a;
        //     size_t temp_idx_b = b;

        //     while (temp_idx_a)
        //     {
        //         morton_a *= 10;
        //         temp_idx_a /= 10;
        //     }

        //     while (temp_idx_b)
        //     {
        //         morton_b *= 10;
        //         temp_idx_b /= 10;
        //     }

        //     morton_a += a;
        //     morton_b += b;
        // }

        // return CountLeadingZerosCombined(morton_a, morton_b);
    // }

    // int calculate_direction(MortonCodeNode* morton_code, const int& first_idx)
    // {
    //     return (number_leading_zeros(morton_code, first_idx, first_idx + 1) - number_leading_zeros(morton_code, first_idx, first_idx - 1) < 0) ? -1 : 1;
    // }
    
    // int upper_range_bound(MortonCodeNode* morton_code, const int& first_idx, const int& dir)
    // {
    //     int min_diff = number_leading_zeros(morton_code, first_idx, first_idx - dir);

        // int upper_range = 2;
        // while (number_leading_zeros(morton_code, first_idx, first_idx + (dir * upper_range)) > min_diff)
        //     upper_range *= 2;

    //     return upper_range;
    // }
    
    // int lower_range_bound(MortonCodeNode* morton_code, const int& first_idx, const int& dir, const int& max_range)
    // {
    //     int min_diff = number_leading_zeros(morton_code, first_idx, first_idx - dir);

        // int lower_range = 0;
        // for (int i = max_range/2; i >= 1; i /= 2)
        // {
        //     if (number_leading_zeros(morton_code, first_idx, first_idx + (min_diff + i) * dir) > min_diff)
        //         lower_range += i;
        // }

    //     return lower_range;
    // }
    
    // int calculate_split_index(MortonCodeNode* morton_code, const int& first_idx, const int& dir, const int& last_idx) 
    // {
        // const uint32_t morton_first = morton_code[first_idx].morton_code;
        // const uint32_t morton_last = morton_code[last_idx].morton_code;

        // if(morton_first == morton_last)
        //     return (first_idx + last_idx) >> 1;

        // int compare_number = __builtin_clz(morton_first ^ morton_last);

        // int split_idx = first_idx;
        // int step = last_idx - first_idx;

        // do
        // {
        //     step = (step + 1) >> 1;
        //     int new_split = split_idx + step;

        //     if(new_split < last_idx && number_leading_zeros(morton_code, first_idx, new_split) > compare_number)
        //     {
        //         split_idx = new_split;
        //     }
            


        // } while(step > 1);

        // return split_idx;
    // }
    
    // Bounds3D calculate_bounds(BVHNode* shapes, MortonCodeNode* morton_code, const int& first_idx, const int& last_idx)
    // {
    //     Bounds3D bound = shapes[morton_code[first_idx].idx_shape].bound;

    //     for (int idx = first_idx + 1; idx <= last_idx; idx++)
                // Union(bound, shapes[morton_code[idx].idx_shape].bound);
        
    //     return bound;
    // }
}