#include "../types.h"
#include "../utils.h"
#include "bvh.h"
#include "jpc_api.h"
#include "log/log.h"
#include "sorting/sort.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t left_shift3(uint32_t x)
{
    x = (x * 0x00010001u) & 0xFF0000FFu;
    x = (x * 0x00000101u) & 0x0F00F00Fu;
    x = (x * 0x00000011u) & 0xC30C30C3u;
    x = (x * 0x00000005u) & 0x49249249u;
    return x;
}

uint32_t encode_morton(vec3 centroid)
{
    uint32_t x
        = left_shift3((uint32_t)MIN(MAX(centroid[0] * 1024.0f, 0.0f), 1023.0f));
    uint32_t y
        = left_shift3((uint32_t)MIN(MAX(centroid[1] * 1024.0f, 0.0f), 1023.0f));
    uint32_t z
        = left_shift3((uint32_t)MIN(MAX(centroid[2] * 1024.0f, 0.0f), 1023.0f));

    return x * 4 + y * 2 + z;
}

#ifdef _MSC_VER
int isinff(float x)
{
    return isinf(x);
}
int isnanf(float x)
{
    return isnan(x);
}
#endif

int count_leading_zeros_combined(uint32_t a, uint32_t b)
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

int number_leading_zeros(uint32_t*    morton_codes,
                         int          idx_a,
                         int          offset,
                         const size_t number_nodes)
{
    int idx_b = idx_a + offset;

    if (idx_b < 0 || (size_t)idx_b > number_nodes)
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

    return count_leading_zeros_combined(morton_a, morton_b);
}
// interval
// [start,end)
uint calc_split_idx(uint32_t* morton, int start, int end, size_t number_nodes)
{
    int      min_idx = start;
    int      max_idx = end - 1;
    uint32_t morton_first = morton[min_idx];
    uint32_t morton_last = morton[max_idx];

    if (morton_first == morton_last)
        return (min_idx + max_idx) >> 1;

    int compare_number
        = count_leading_zeros_combined(morton_first, morton_last);

    int split_idx = min_idx;
    int step = max_idx - min_idx;

    do
    {
        step = (step + 1) >> 1;
        int new_split_idx = split_idx + step;

        if (new_split_idx < max_idx
            && number_leading_zeros(
                   morton, min_idx, new_split_idx - min_idx, number_nodes)
                   > compare_number)
        {
            split_idx = new_split_idx;
        }

    } while (step > 1);

    return split_idx;
}

void bounds3d_t_merge(bounds3d_t* bounds, uint n, bounds3d_t* dst)
{
    assert(n > 0);
    dst[0] = bounds[0];
    for (uint i = 1; i < n; i++)
    {
        glm_aabb_merge((vec3*)(bounds + i), (vec3*)dst, (vec3*)dst);
    }
}
// interval
// [start,end)
// returns how many childs where found
int eval_child_intervalls(int       start,
                          int       end,
                          uint32_t* morton,
                          uint      max_nodes,
                          uint*     intervalls)
{

    uint max_num_childs = MIN(BVH_WIDTH, end - start);
    uint num_childs = 1;
    do
    {
        // find max intervall
        uint max_size = 0;
        uint max_intervall_i = 0;
        for (uint i = 0; i < num_childs; i++)
        {
            uint size = intervalls[i + 1] - intervalls[i];
            if (max_size < size)
            {
                max_intervall_i = i;
                max_size = size;
            }
        }
        // shift
        for (uint i = num_childs + 1; i > max_intervall_i + 1; i--)
        {
            assert(i < BVH_WIDTH + 1);
            intervalls[i] = intervalls[i - 1];
        }

        intervalls[max_intervall_i + 1]
            = calc_split_idx(morton,
                             intervalls[max_intervall_i],
                             intervalls[max_intervall_i + 2],
                             max_nodes)
              + 1;
        num_childs++;

    } while (num_childs < max_num_childs);
    return max_num_childs;
}

void bvh_nodes_fill_empty(bvh_node_t* nodes, uint count)
{
    for (uint i = 0; i < count; i++)
    {
        nodes[i].is_leaf = true;
        nodes[i].idx = 0;
        for (int j = 0; j < 3; j++)
            nodes[i].bound.min[j] = INFINITY;

        for (int j = 0; j < 3; j++)
            nodes[i].bound.max[j] = INFINITY;
    }
}

// interval
// [start,end)
bvh_node_t lbvh_build_recursiv(uint        start,
                               uint        end,
                               bvh_tree_t* tree,
                               uint32_t*   morton,
                               bounds3d_t* bounds,
                               uint*       permutation,
                               uint        max_nodes)
{
    assert(start <= end);
    assert(start != end);
    if (start + 1 == end)
    {
        // assert(fabs(bounds[start].min[0]) > 0.001);
        return (bvh_node_t){
            .bound = bounds[start],
            .idx = permutation[start],
            .is_leaf = true,
        };
    }

    // always split the largest intervall
    uint childs_intervalls[BVH_WIDTH + 1] = {start, end};

    uint num_childs = eval_child_intervalls(
        start, end, morton, max_nodes, childs_intervalls);

    int node_id = bvh_push_back(tree);

    bvh_node_t child_nodes[BVH_WIDTH];
    uint       i = 0;
    for (; i < num_childs; i++)
    {
        child_nodes[i] = lbvh_build_recursiv(childs_intervalls[i],
                                             childs_intervalls[i + 1],
                                             tree,
                                             morton,
                                             bounds,
                                             permutation,
                                             max_nodes);
    }

    bvh_nodes_fill_empty(child_nodes + i, BVH_WIDTH - i);
    bvh_set_node(tree, node_id, child_nodes);

    bounds3d_t node_bound = child_nodes[0].bound;
    for (uint i = 1; i < num_childs; i++)
    {
        glm_aabb_merge((vec3*)(&child_nodes[i].bound),
                       (vec3*)&node_bound,
                       (vec3*)&node_bound);
    }
    assert(!isnanf(node_bound.min[0]));

    assert(!isinff(node_bound.min[0]));
    return (bvh_node_t){.bound = node_bound, .idx = node_id, .is_leaf = false};
}

bvh_tree_t* lbvh_build(arena_t*    arena,
                       uint        n,
                       bounds3d_t* bounds,
                       vec3*       centers)
{
    if (n == 0)
        return NULL;
    uint*     permutation = malloc(sizeof(uint) * n);
    uint32_t* morton = malloc(sizeof(uint32_t) * n);

    bvh_tree_t* tree = bvh_create(arena, n, n); // overestimate?

    for (uint i = 0; i < n; i++)
    {
        morton[i] = encode_morton(centers[i]);
    }

    // make morton code unique
    for (uint i = 1; i < n; i++)
    {
        int diff = MAX((long)morton[i - 1] - (long)morton[i] + 1, 0);
        // morton[i] >= morton[i] + (-morton[i] + morton[i-1]+1)=[morton[i-1]]+1
        // morton[i] >= morton[i]+0
        morton[i] += diff;
    }

    sort_permutation_uint(morton, permutation, n);
    bvh_node_t root = lbvh_build_recursiv(
        0, n, tree, morton, bounds, permutation, n - BVH_WIDTH);
    tree->root_bound = root.bound;
    if (root.is_leaf)
    {
        uint       id = bvh_push_back(tree);
        bvh_node_t nodes[BVH_WIDTH];
        nodes[0] = root;
        bvh_nodes_fill_empty(nodes + 1, BVH_WIDTH - 1);
        bvh_set_node(tree, id, nodes);
    }

    free(morton);
    free(permutation);
    return tree;
}
