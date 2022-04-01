#pragma once
#include "stdbool.h"
#include <assert.h>
#include <stdint.h>

typedef struct bvh_node_intern_s bvh_node_intern_t;

typedef struct
{
    uint32_t is_leaf; // always 1
    uint32_t idx;
} bvh_leaf_ref_t;

typedef struct
{
    const bvh_node_intern_t* ptr; // 16 bit allined
} bvh_inode_ref_t;

typedef struct
{
    union {
        uint64_t        data;
        bvh_leaf_ref_t  leaf;
        bvh_inode_ref_t inode;
    };
} bvh_node_ref_t;

static inline bool is_leaf(bvh_node_ref_t ref)
{
    return ref.data & 1;
}

static inline const bvh_node_intern_t* get_inode(bvh_node_ref_t ref)
{
    assert(!is_leaf(ref));
    return ref.inode.ptr;
}

static inline uint32_t get_leaf_idx(bvh_node_ref_t ref)
{
    assert(is_leaf(ref));
    return ref.leaf.idx;
}
