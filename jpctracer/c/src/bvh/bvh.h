#pragma once
#include "../config.h"
#include "../types.h"
#include "bounds3d.h"
#include "bvh_ref.h"
#include "jpc_api.h"
#include "shapes.h"

#define SIMD_WIDTH BVH_WIDTH
#include "boundsN.h"

typedef struct
{
    uint32_t   idx;
    bool       is_leaf;
    bounds3d_t bound;
} bvh_node_t;

typedef struct bvh_tree_s
{
    uint               nodes_max_count;
    uint               nodes_count;
    bvh_node_intern_t* nodes;
    bounds3d_t         root_bound;

} bvh_tree_t;

bvh_tree_t* bvh_create(arena_t* arena, int max_nodes, int max_leafs);

void bvh_log(const bvh_tree_t* tree);

typedef struct
{
    float          min_distance;
    bvh_node_ref_t node;
} bvh_stack_item_cl_t;

void bvh_set_node(bvh_tree_t* tree, uint id, const bvh_node_t* childs);

uint bvh_push_back(bvh_tree_t* tree);

void bvh_finish(bvh_tree_t* tree);

void bvh_free(bvh_tree_t tree);

typedef struct
{

    bvh_stack_item_cl_t stack_data[64];

    bvh_stack_item_cl_t* stack_begin;
    bvh_stack_item_cl_t* stack;
    float                min_distance;
    ray_trav_bounds_t    ray;
    ray_trav_boundsN_t ray_boundsN;
    
} bvh_intersector_closest_t;



bool bvh_intersect_closest_init(const bvh_tree_t*          tree,
                                const ray_t*               ray,
                                bvh_intersector_closest_t* result);

bool find_closest_leaf(int*                       id,
                       bvh_intersector_closest_t* intersector,
                       float                      max_distance);

typedef struct
{
    bvh_node_ref_t    stack_data[64];
    bvh_node_ref_t*   stack_begin;
    bvh_node_ref_t*   stack;
    float             min_distance;
    float max_distance;
    ray_trav_bounds_t ray;
    ray_trav_boundsN_t ray_boundsN;
} bvh_intersector_any_t;

bool bvh_intersect_any_init(const bvh_tree_t*      tree,
                            const ray_t*           ray,
                            bvh_intersector_any_t* result);

bool find_any_leaf(int*                   id,
                   bvh_intersector_any_t* intersector);
