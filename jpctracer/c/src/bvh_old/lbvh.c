#include "../utils.h"
#include "bvh.h"
#include "jpc_api.h"
#include "log/log.h"
#include "sorting/sort.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t left_shift3_old(uint32_t x)
{
    x = (x * 0x00010001u) & 0xFF0000FFu;
    x = (x * 0x00000101u) & 0x0F00F00Fu;
    x = (x * 0x00000011u) & 0xC30C30C3u;
    x = (x * 0x00000005u) & 0x49249249u;
    return x;
}

uint32_t encode_morton_old(vec3 centroid)
{
    uint32_t x
        = left_shift3_old(MIN(MAX(centroid[0] * 1024.0f, 0.0f), 1023.0f));
    uint32_t y
        = left_shift3_old(MIN(MAX(centroid[1] * 1024.0f, 0.0f), 1023.0f));
    uint32_t z
        = left_shift3_old(MIN(MAX(centroid[2] * 1024.0f, 0.0f), 1023.0f));

    return x * 4 + y * 2 + z;
}

int count_leading_zeros_combined_old(uint32_t a, uint32_t b)
{
    const uint32_t combined = a ^ b;

    if (combined == 0)
        return 32U;

#if defined(__clang__) || defined(__GNUC__)
    return __builtin_clz(combined);
#elif defined _MSC_VER
    return __lzcnt(combined);
#else
    assert(false);
    return 0;
#endif
}
int number_leading_zeros_old(uint32_t*    morton_codes,
                             int          idx_a,
                             int          offset,
                             const size_t number_nodes)
{
    int idx_b = idx_a + offset;

    if (idx_b < 0 || idx_b > number_nodes)
        return -1;

    uint32_t morton_a = morton_codes[idx_a];
    uint32_t morton_b = morton_codes[idx_b];

    if (morton_a == morton_b)
    {
        size_t temp_idx_a = idx_a;
        size_t temp_idx_b = idx_b;

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

        morton_a += idx_a;
        morton_b += idx_b;
    }

    return count_leading_zeros_combined_old(morton_a, morton_b);
}

int calc_direction(uint32_t* morton, int idx, size_t number_nodes)
{
    int dir_forward = number_leading_zeros_old(morton, idx, 1, number_nodes);
    int dir_backward = number_leading_zeros_old(morton, idx, -1, number_nodes);
    return (dir_forward - dir_backward < 0) ? -1 : 1;
}

uint calc_split_idx_old(uint32_t* morton,
                        int       min_idx,
                        int       max_idx,
                        int       dir,
                        size_t    number_nodes)
{
    uint32_t morton_first = morton[min_idx];
    uint32_t morton_last = morton[max_idx];

    if (morton_first == morton_last)
        return (min_idx + max_idx) >> 1;

    int compare_number
        = count_leading_zeros_combined_old(morton_first, morton_last);

    int split_idx = min_idx;
    int step = max_idx - min_idx;

    do
    {
        step = (step + 1) >> 1;
        int new_split_idx = split_idx + step;

        if (new_split_idx < max_idx
            && number_leading_zeros_old(
                   morton, min_idx, new_split_idx - min_idx, number_nodes)
                   > compare_number)
        {
            split_idx = new_split_idx;
        }

    } while (step > 1);

    return split_idx;
}

int calc_last_idx(uint32_t* morton, int idx, int dir, size_t number_nodes)
{
    int min_diff = number_leading_zeros_old(morton, idx, -dir, number_nodes);

    int upper_range = 2;
    while (
        number_leading_zeros_old(morton, idx, dir * upper_range, number_nodes)
        > min_diff)
        upper_range *= 2;

    int lower_range = 0;
    for (int i = upper_range / 2; i >= 1; i /= 2)
    {
        if (number_leading_zeros_old(
                morton, idx, (lower_range + i) * dir, number_nodes)
            > min_diff)
            lower_range += i;
    }

    return idx + (lower_range * dir);
}

void lbvh_build_old(bvh_tree_t tree, vec3* centers, uint* permutation)
{
    log_info("build lbvh");
    const size_t nodes_n = tree.n - 1;

    uint32_t* morton = malloc(sizeof(uint32_t) * tree.n);
    for (int i = 0; i < tree.n; i++)
    {
        morton[i] = encode_morton_old(centers[i]);
    }

    sort_permutation_uint(morton, permutation, tree.n);

    apply_permutation(
        permutation, tree.shape_bounds, tree.n, sizeof(bounds3d_t));
    // make morton code unique
    for (int i = 1; i < tree.n; i++)
    {
        int diff = MAX((long)morton[i - 1] - (long)morton[i] + 1, 0);
        // morton[i] >= morton[i] + (-morton[i] + morton[i-1]+1)=[morton[i-1]]+1
        // morton[i] >= morton[i]+0
        morton[i] += diff;
    }
    // build binary radix tree
    // #pragma omp parallel for
    for (int idx = 0; idx < nodes_n; idx++)
    {
        int direction = calc_direction(morton, idx, nodes_n);

        uint last_idx = calc_last_idx(morton, idx, direction, nodes_n);

        int min_idx = MIN(idx, last_idx);
        int max_idx = MAX(idx, last_idx);

        uint split_idx
            = calc_split_idx_old(morton, min_idx, max_idx, direction, nodes_n);

        bounds3d_t* bounds = tree.shape_bounds + min_idx;
        bounds3d_t_merge_old(
            bounds, max_idx - min_idx + 1, tree.node_bounds + idx);

        bvh_node_t node = {
            .first_idx = min_idx,
            .last_idx = max_idx,
            .split_idx = split_idx,
        };
        tree.nodes[idx] = node;
    }

    free(morton);
}
