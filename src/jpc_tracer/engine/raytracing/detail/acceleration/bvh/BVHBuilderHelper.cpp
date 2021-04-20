#include "BVHBuilderHelper.h"
#include <stdint.h>



namespace jpctracer::raytracing
{    
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

        int compare_number = clz(morton_first ^ morton_last); 

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

        while(++bound_begin <= bound_end)
            Union(bound, *bound_begin);

        return bound;
    }
}