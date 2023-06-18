
#include "bvh.h"
#include "../utils.h"
#include "bvh_ref.h"
#include "jpc_api.h"
#include <stdlib.h>
#include <string.h>
#include <xmmintrin.h>

typedef struct bvh_node_intern_s
{
    boundsN_t      bounds;
    bvh_node_ref_t childs[BVH_WIDTH];
} bvh_node_intern_t;

bvh_tree_t* bvh_create(arena_t* arena, int max_nodes, int max_leafs)
{
    assert(max_nodes <= max_leafs);
    arena_reserve(
        arena, sizeof(bvh_tree_t) + 16 + sizeof(bvh_node_intern_t) * max_nodes);

    bvh_tree_t* dst = arena_alloc(arena, sizeof(bvh_tree_t));

    dst->nodes
        = arena_alloc_aligned(arena, sizeof(bvh_node_intern_t) * max_nodes, 16);
    dst->nodes_max_count = max_nodes;
    dst->nodes_count = 0;
    return dst;
}

bvh_node_ref_t node_ref_create(const bvh_node_t* node, bvh_node_intern_t* nodes)
{
    bvh_node_ref_t result;
    if (node->is_leaf)
    {
        result.leaf.is_leaf = 1;
        result.leaf.idx = node->idx;
    }
    else
    {
        result.inode.ptr = nodes + node->idx;
        assert(!is_leaf(result));
    }
    return result;
}

bvh_node_ref_t node_ref_move(bvh_node_ref_t           ref,
                             const bvh_node_intern_t* old_nodes,
                             const bvh_node_intern_t* new_nodes)
{
    if (!is_leaf(ref))
    {
        ref.inode.ptr = ref.inode.ptr - old_nodes + new_nodes;
        assert(!is_leaf(ref));
    }
    return ref;
}

bool check_node(const bvh_tree_t* tree, const bvh_node_t* node)
{
    if (!node->is_leaf)
    {
        return node->idx < tree->nodes_count;
    }
    return true;
}

void bvh_set_node(bvh_tree_t* tree, uint id, const bvh_node_t* childs)
{
    for (int i = 0; i < BVH_WIDTH; i++)
    {
        assert(check_node(tree, &childs[i]));
        // LOG_BOUNDS("bound", childs[i].bound.min, childs[i].bound.max);
    }
    assert(id < tree->nodes_max_count);
    for (int i = 0; i < 3; i++)
    {

        // min
        for (int j = 0; j < BVH_WIDTH; j++)
        {
            float tmp = childs[j].bound.min[i];
            tree->nodes[id].bounds.borders[2 * i].data[j] = tmp;
        }
        // max
        for (int j = 0; j < BVH_WIDTH; j++)
            tree->nodes[id].bounds.borders[2 * i + 1].data[j]
                = childs[j].bound.max[i];
    }
    for (int i = 0; i < BVH_WIDTH; i++)
    {
        tree->nodes[id].childs[i] = node_ref_create(&childs[i], tree->nodes);
    }
}

uint bvh_push_back(bvh_tree_t* tree)
{
    assert(tree->nodes_count < tree->nodes_max_count);
    uint result = tree->nodes_count;
    tree->nodes_count++;
    return result;
}

void bvh_nodes_copy_to(const bvh_node_intern_t* src_nodes, bvh_tree_t* dst)
{
    memcpy(dst->nodes, src_nodes, sizeof(bvh_node_intern_t) * dst->nodes_count);
    for (uint i = 0; i < dst->nodes_count; i++)
    {
        for (int j = 0; j < BVH_WIDTH; j++)
        {
            bvh_node_ref_t* ref = &dst->nodes[i].childs[j];
            *ref = node_ref_move(*ref, src_nodes, dst->nodes);
        }
    }
}

bvh_tree_t* bvhtree_copy(arena_t* arena, const bvh_tree_t* tree)
{
    bvh_tree_t* dst
        = bvh_create(arena, tree->nodes_count, tree->nodes_max_count);
    dst->nodes_count = tree->nodes_count;
    bvh_nodes_copy_to(tree->nodes, dst);
    memcpy(&dst->root_bound, &tree->root_bound, sizeof(tree->root_bound));
    return dst;
}

void sort2(bvh_stack_item_cl_t* first, bvh_stack_item_cl_t* second)
{
    if (second->min_distance > first->min_distance) // last < first
    {
        bvh_stack_item_cl_t tmp = *first;
        *first = *second;
        *second = tmp;
    }
}

// T: Random access iterator with a semiregular valuetype
void sort3(bvh_stack_item_cl_t* first)
{
    sort2(first + 0, first + 1);
    sort2(first + 1, first + 2);
    sort2(first + 0, first + 1);
}

void sort4(bvh_stack_item_cl_t* first)
{
    sort2(first + 0, first + 1);
    sort2(first + 2, first + 3);
    sort2(first + 0, first + 2);
    sort2(first + 1, first + 3);
    sort2(first + 1, first + 2);
}

void insertion_sort(bvh_stack_item_cl_t* first, bvh_stack_item_cl_t* last)
{
    int size = last - first;

    // Insertion Sort
    for (int i = 1; i < size; i++)
    {

        bvh_stack_item_cl_t temp = first[i];
        int                 j = i - 1;

        while ((temp.min_distance > first[j].min_distance) && (j >= 0))
        {

            // swap
            first[j + 1] = first[j];
            j = j - 1;
        }

        first[j + 1] = temp;
    }
}

int next_mask_idx(int* mask)
{
#ifdef _MSC_VER
    int r = _tzcnt_u32(*mask);
#else
    int r = __builtin_ctz(*mask);
#endif

    *mask &= *mask - 1;
    return r;
}

bvh_stack_item_cl_t* hit_push_to_stack(hits_boundsN_t*          hits,
                                       bvh_stack_item_cl_t*     stack,
                                       const bvh_node_intern_t* node)
{
    int i = next_mask_idx(&hits->mask);
    stack->min_distance = hits->min.data[i];
    stack->node = node->childs[i];
    stack++;
    return stack;
}

bvh_stack_item_cl_t* bounds_intersect_closest(const bvh_node_intern_t*  node,
                                              const ray_trav_boundsN_t* ray,
                                              bvh_stack_item_cl_t*      stack)
{
    hits_boundsN_t hits = bounds3d_intersectN(ray, &node->bounds);

    if (hits.mask == 0)
    {
        return stack;
    }
    bvh_stack_item_cl_t* stack_begin = stack;

    stack = hit_push_to_stack(&hits, stack, node);

    if (hits.mask == 0)
    {
        return stack;
    }

    stack = hit_push_to_stack(&hits, stack, node);
#if BVH_WIDTH == 2

    sort2(stack_begin, stack_begin + 1);
#endif

#if BVH_WIDTH >= 4
    if (hits.mask == 0)
    {
        sort2(stack_begin, stack_begin + 1);
        return stack;
    }
    stack = hit_push_to_stack(&hits, stack, node);

    if (hits.mask == 0)
    {
        sort3(stack_begin);
        return stack;
    }

    stack = hit_push_to_stack(&hits, stack, node);

#endif
#if BVH_WIDTH == 4
    sort4(stack_begin);
#endif
#if BVH_WIDTH > 4
    if (hits.mask == 0)
    {
        sort4(stack_begin);
        return stack;
    }
    for (int i = 4; i < 8 && hits.mask != 0; i++)
    {
        stack = hit_push_to_stack(&hits, stack, node);
    }
    insertion_sort(stack_begin, stack);
#endif

    return stack;
}

bvh_node_ref_t* bounds_intersect_any(const bvh_node_intern_t*  node,
                                     const ray_trav_boundsN_t* ray,
                                     bvh_node_ref_t*           stack)
{
    hits_boundsN_t hits = bounds3d_intersectN(ray, &node->bounds);

    for (int i = 0; i < BVH_WIDTH; i++)
    {
        if (hits.mask == 0)
            return stack;

        int j = next_mask_idx(&hits.mask);
        *stack = node->childs[j];
        stack++;
    }
    return stack;
}

bool find_closest_leaf(int*                       id,
                       bvh_intersector_closest_t* intersector,
                       float                      max_distance)
{
    bvh_intersector_closest_t* i = intersector;
    bvh_stack_item_cl_t        item;

    i->ray_boundsN.t_max = simd_set_ps1(max_distance);

    while (i->stack != i->stack_begin)
    {
        i->stack--;
        item = *i->stack;

        if (item.min_distance < max_distance)
        {
            bvh_stack_item_cl_t* stack_begin = i->stack;
            while (!is_leaf(item.node))
            {
                i->stack = bounds_intersect_closest(
                    get_inode(item.node), &i->ray_boundsN, i->stack);

                if (stack_begin != i->stack)
                {
                    i->stack--;
                    item = *i->stack;
                }
                else
                {
                    break;
                }
            }

            if (is_leaf(item.node))
            {
                *id = get_leaf_idx(item.node);
                return true;
            }
        };
    }
    return false;
}

bool find_any_leaf(int* id, bvh_intersector_any_t* intersector)
{

    bvh_intersector_any_t* i = intersector;
    bvh_node_ref_t         item;
    /*
    ray_trav_boundsN_t     ray
        = ray_trav_boundsN_make(&i->ray, 0.f, i->max_distance);*/

    while (i->stack != i->stack_begin)
    {
        i->stack--;
        item = *i->stack;

        if (is_leaf(item))
        {
            *id = get_leaf_idx(item);
            return true;
        }
        i->stack
            = bounds_intersect_any(get_inode(item), &i->ray_boundsN, i->stack);
    }

    return false;
}
bvh_node_ref_t bvh_get_root(const bvh_tree_t* tree)
{

    bvh_node_t root = {
        .idx = 0,
        .is_leaf = false,
    };
    return node_ref_create(&root, tree->nodes);
}

bool bvh_intersect_closest_init(const bvh_tree_t*          tree,
                                const ray_t*               ray,
                                bvh_intersector_closest_t* result)
{
    result->stack = result->stack_data;
    result->stack_begin = result->stack_data;
    result->stack->node = bvh_get_root(tree);
    result->stack->min_distance = 0 + ERROR_THICKNESS;
    result->stack++;
    result->min_distance = 0;
    result->ray = ray_trav_bounds_make(ray);
    result->ray_boundsN
        = ray_trav_boundsN_make(&result->ray, 0.f, ray->clip_end);
    /*
    bounds3d_t* bounds = tree.n == 1 ? tree.shape_bounds : tree.node_bounds;
    intervall_t hit = bounds3d_intersect(bounds, &result->ray);
    intervall_t search_intervall = {min_distance, INFINITY};
    return does_intersect_intervall(&hit, &search_intervall);*/
    return true;
}

bool bvh_intersect_any_init(const bvh_tree_t*      tree,
                            const ray_t*           ray,
                            bvh_intersector_any_t* result)
{
    result->stack = result->stack_data;
    result->stack_begin = result->stack_data;
    *result->stack = bvh_get_root(tree);
    result->stack++;
    result->min_distance = 0;
    result->max_distance = ray->clip_end;
    result->ray = ray_trav_bounds_make(ray);
    result->ray_boundsN
        = ray_trav_boundsN_make(&result->ray, 0.f, ray->clip_end);
    return true;
}
// returnes the identifier
int bvh_node_log_recursive(const bvh_inode_ref_t ref, int depth, int identifier)
{
    for (int i = 0; i < BVH_WIDTH; i++)
    {

        char           name[256];
        bvh_node_ref_t child_ref = ref.ptr->childs[i];
        if (is_leaf(child_ref))
        {

            sprintf(name,
                    "%d_%d_D_%d_leaf_%d",
                    identifier,
                    i,
                    depth,
                    child_ref.leaf.idx);
            if (isinff(ref.ptr->bounds.borders[0].data[i]))
            {
                sprintf(name + strlen(name), "_empty");
            }
        }
        else
        {
            sprintf(name, "%d_%d_D_%d_inode", identifier, i, depth);
        }
        bounds4_log(name, &ref.ptr->bounds, i);
    }
    for (int i = 0; i < BVH_WIDTH; i++)
    {
        if (!is_leaf(ref.ptr->childs[i]))
        {
            identifier = bvh_node_log_recursive(
                ref.ptr->childs[i].inode, depth + 1, identifier + 1);
        }
    }
    return identifier;
}

void bvh_log(const bvh_tree_t* tree)
{
    bvh_node_ref_t root = bvh_get_root(tree);
    if (is_leaf(root))
    {
        LOG_BOUNDS("root", tree->root_bound.min, tree->root_bound.max);
    }
    else
    {
        bvh_node_log_recursive(root.inode, 0, 0);
    }
}
