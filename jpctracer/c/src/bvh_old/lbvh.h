#pragma once
#include "bvh.h"
#include "intersect.h"
#include <math.h>
#define unlikely(expr) __builtin_expect(!!(expr), 0)
#define likely(expr) __builtin_expect(!!(expr), 1)
//#define forceinline __attribute__((always_inline)) static
#define forceinline static inline

forceinline int left_node_idx(bvh_node_t node)
{
    return node.split_idx;
}

forceinline int right_node_idx(bvh_node_t node)
{
    return node.split_idx + 1;
}

forceinline bool is_left_leaf(bvh_node_t node)
{
    return left_node_idx(node) == node.first_idx;
}

forceinline bool is_right_leaf(bvh_node_t node)
{
    return right_node_idx(node) == node.last_idx;
}

typedef struct
{
    float min_distance;
    int   id;
    bool  is_leaf;
} bvh_stack_item_cl_t;

typedef struct
{
    int  id;
    bool is_leaf;
} bvh_stack_item_an_t;

forceinline bvh_stack_item_cl_t* add_to_stack_cl(
    int                      id,
    bool                     is_leaf,
    const bvh_tree_t*        tree,
    const ray_trav_bounds_t* ray,
    const intervall_t*       search_intervall,
    bvh_stack_item_cl_t*     stack)
{
    if (id == 53)
    {
        // printf("\n");
    }
    bounds3d_t* bounds = is_leaf ? tree->shape_bounds : tree->node_bounds;
#ifdef LOG_TRAVERSAL
    printf("Bounds intersect: \n");
    printf("Bounds: ");
    printf_arrayf(3, bounds[id].min);
    printf_arrayf(3, bounds[id].max);
    printf("\n");
    printf("ray orig: ");
    printf_arrayf(3, ray.origin);
    printf(" inv dir: ");
    printf_arrayf(3, ray.inverse_direction);
    printf("\n");
    printf("Max: distance %f\n", search_intervall.max);
#endif
    if (id < 0 && id > 1000)
    {
        // printf("Komisch\n");
    }
    intervall_t hit = bounds3d_intersect(bounds + id, ray);
#ifdef LOG_TRAVERSAL
    printf("t_min: %f\nt_max: %f\n", hit.min, hit.max);
#endif
    if (does_intersect_intervall(&hit, search_intervall))
    {

#ifdef LOG_TRAVERSAL
        printf("Did intersect\n");
#endif
        stack->min_distance = hit.min;
        stack->is_leaf = is_leaf;
        stack->id = id;
        stack++;
        return stack;
    }

#ifdef LOG_TRAVERSAL
    printf("Did not intersect\n");
#endif
    return stack;
}

static inline bvh_stack_item_an_t* add_to_stack_an(
    int                      id,
    bool                     is_leaf,
    const bvh_tree_t*        tree,
    const ray_trav_bounds_t* ray,
    const intervall_t*       search_intervall,
    bvh_stack_item_an_t*     stack)
{

    bounds3d_t* bounds = is_leaf ? tree->shape_bounds : tree->node_bounds;
    intervall_t hit = bounds3d_intersect(bounds + id, ray);
    if (does_intersect_intervall(&hit, search_intervall))
    {
        stack->is_leaf = is_leaf;
        stack->id = id;
        stack++;
        return stack;
    }
    return stack;
}

forceinline bvh_stack_item_cl_t* bounds_intersect_closest(
    int                      id,
    const bvh_tree_t*        tree,
    const ray_trav_bounds_t* ray,
    const intervall_t*       search_intervall,
    bvh_stack_item_cl_t*     stack_begin)
{

    bvh_node_t node = tree->nodes[id];

    bvh_stack_item_cl_t* stack = stack_begin;
    /*
    stack = add_to_stack_cl(right_node_idx(node),
                            is_right_leaf(node),
                            tree,
                            ray,
                            search_intervall,
                            stack);
    stack = add_to_stack_cl(left_node_idx(node),
                            is_left_leaf(node),
                            tree,
                            ray,
                            search_intervall,
                            stack);
                            */
    // this has less cache misses
    int  left_child = left_node_idx(node);
    int  right_child = right_node_idx(node);
    bool left_leaf = is_left_leaf(node);
    bool right_leaf = is_right_leaf(node);
    if (left_child == 10 || right_child == 10)
    {
        int test = 0;
    }

    bounds3d_t* left_bounds
        = left_leaf ? tree->shape_bounds : tree->node_bounds;
    bounds3d_t* right_bounds
        = right_leaf ? tree->shape_bounds : tree->node_bounds;

    intervall_t hit1 = bounds3d_intersect(&left_bounds[left_child], ray);

    intervall_t hit2 = bounds3d_intersect(&right_bounds[right_child], ray);

    if (does_intersect_intervall(&hit1, search_intervall))
    {
        stack->id = left_child;
        stack->is_leaf = left_leaf;
        stack->min_distance = hit1.min;
        stack++;
    }

    if (does_intersect_intervall(&hit2, search_intervall))
    {
        stack->id = right_child;
        stack->is_leaf = right_leaf;
        stack->min_distance = hit2.min;
        stack++;
    }

    if (stack - stack_begin == 2
        && stack_begin[0].min_distance < stack_begin[1].min_distance)
    {
        bvh_stack_item_cl_t tmp = stack_begin[0];
        stack_begin[0] = stack_begin[1];
        stack_begin[1] = tmp;
    }
    return stack;
}

static inline bvh_stack_item_an_t* bounds_intersect_any(
    int                      id,
    const bvh_tree_t*        tree,
    const ray_trav_bounds_t* ray,
    const intervall_t*       search_intervall,
    bvh_stack_item_an_t*     stack)
{
    bvh_node_t node = tree->nodes[id];

    stack = add_to_stack_an(left_node_idx(node),
                            is_left_leaf(node),
                            tree,
                            ray,
                            search_intervall,
                            stack);

    stack = add_to_stack_an(right_node_idx(node),
                            is_right_leaf(node),
                            tree,
                            ray,
                            search_intervall,
                            stack);
    return stack;
}

typedef struct
{

    bvh_stack_item_cl_t stack_data[64];

    bvh_stack_item_cl_t* stack_begin;
    bvh_stack_item_cl_t* stack;
    float                min_distance;
    bvh_tree_t           tree;
    ray_trav_bounds_t    ray;
} bvh_intersetor_closest_t;

forceinline bool bvh_intersect_init(bvh_tree_t                tree,
                                    float                     min_distance,
                                    ray_trav_t                ray,
                                    bvh_intersetor_closest_t* result)
{
    result->tree = tree;
    result->stack = result->stack_data;
    result->stack_begin = result->stack_data;
    result->stack->id = 0;
    result->stack->is_leaf = tree.n == 1;
    result->stack->min_distance = min_distance + ERROR_THICKNESS;
    result->stack++;
    result->min_distance = min_distance;
    result->ray = ray_trav_bounds_make_old(ray);
    /*
    bounds3d_t* bounds = tree.n == 1 ? tree.shape_bounds : tree.node_bounds;
    intervall_t hit = bounds3d_intersect(bounds, &result->ray);
    intervall_t search_intervall = {min_distance, INFINITY};
    return does_intersect_intervall(&hit, &search_intervall);*/
    return true;
}

__attribute__((noinline)) static bool find_closest_leaf(
    int*                      id,
    bvh_intersetor_closest_t* intersector,
    float                     max_distance)
{
    bvh_intersetor_closest_t* i = intersector;
    bvh_stack_item_cl_t       item;
    intervall_t               intervall = {i->min_distance, max_distance};

    int _test_id = 5; //

    while (i->stack != i->stack_begin)
    {
        i->stack--;
        item = *i->stack;

        if (item.min_distance < max_distance)
        {
            bvh_stack_item_cl_t* stack_begin = i->stack;
            while (!item.is_leaf)
            {
                i->stack = bounds_intersect_closest(
                    item.id, &i->tree, &i->ray, &intervall, i->stack);

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

            if (item.is_leaf)
            {
                *id = item.id;
                return true;
            }
        };
    }
    return false;
}

typedef bool (*intersect_f2)(void* restrict params,
                             const ray_trav_t* ray,
                             intervall_t       intervall,
                             int               id,
                             float* restrict out_max_dist);

__attribute__((always_inline)) static bool find_closest_leaf2(
    const bvh_tree_t* tree,
    const ray_trav_t* ray,
    intervall_t       intervall,
    intersect_f2      intersect,
    void*             params)
{
    bvh_stack_item_cl_t  item;
    bvh_stack_item_cl_t  stack_data[64];
    bvh_stack_item_cl_t* stack = stack_data;
    bvh_stack_item_cl_t* stack_begin = stack_data;
    ray_trav_bounds_t    ray_trav = ray_trav_bounds_make_old(*ray);

    stack->id = 0;
    stack->is_leaf = tree->n == 1;
    stack->min_distance = intervall.min + ERROR_THICKNESS;
    stack++;

    int  _test_id = 5; //
    bool did_intersect = false;

    while (stack != stack_begin)
    {
        stack--;
        item = *stack;

        if (item.min_distance < intervall.max)
        {
            bvh_stack_item_cl_t* stack_begin_tmp = stack;
            while (!item.is_leaf)
            {
                stack = bounds_intersect_closest(
                    item.id, tree, &ray_trav, &intervall, stack);

                if (stack_begin_tmp != stack)
                {
                    stack--;
                    item = *stack;
                }
                else
                {
                    break;
                }
            }

            if (item.is_leaf)
            {
                did_intersect |= intersect(
                    params, ray, intervall, item.id, &intervall.max);
            }
        };
    }
    return did_intersect;
}
typedef struct
{
    bvh_stack_item_an_t  stack_data[64];
    bvh_stack_item_an_t* stack;
    bvh_stack_item_an_t* stack_begin;
    bvh_tree_t           tree;
    ray_trav_bounds_t    ray;
} bvh_intersetor_any_t;

static inline void bvh_intersect_any_init(bvh_tree_t            tree,
                                          ray_trav_t            ray,
                                          bvh_intersetor_any_t* result)
{
    result->tree = tree;
    result->stack = result->stack_data;
    result->stack_begin = result->stack_data;
    result->stack->id = 0;
    result->stack->is_leaf = tree.n == 1;
    result->stack++;
    result->ray = ray_trav_bounds_make_old(ray);
}

static inline bool find_any_leaf(int*                  id,
                                 bvh_intersetor_any_t* intersector,
                                 intervall_t           intervall)
{
    bvh_intersetor_any_t* i = intersector;
    while (i->stack_begin != i->stack)
    {
        i->stack--;
        bvh_stack_item_an_t item = *i->stack;
        if (item.is_leaf)
        {
            *id = item.id;
            return true;
        }
        i->stack = bounds_intersect_any(
            item.id, &i->tree, &i->ray, &intervall, i->stack);
    }
    return false;
}
typedef struct
{
    int                 geom_id;
    triangle_hitpoint_t geom_hit;
} instance_hitpoint_t;

bool spheres_intesect_closest(const ray_trav_t* ray,
                              const spheres_t*  sphs,
                              intervall_t       intervall,
                              const bvh_tree_t* tree,
                              int               index_offset,
                              int*              out_id,
                              float*            out_distance);

bool instances_intersect_closest_old(const ray_trav_t*    world_ray,
                                     const geometries_t*  geom,
                                     intervall_t          intervall,
                                     int*                 id,
                                     instance_hitpoint_t* result);
bool ray_intersect_c(const geometries_t* geometries,
                     ray_t*              ray,
                     hit_point_t*        out_hitpoint);

bool ray_intersect_c2(const geometries_t* geometries,
                      ray_t*              ray,
                      hit_point_t*        out_hitpoint);