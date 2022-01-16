#include "bvh.h"
#include "jpc_api.h"

int left_node_idx(bvh_node_t node)
{
    return node.split_idx;
}

int right_node_idx(bvh_node_t node)
{
    return node.split_idx + 1;
}

bool is_left_leaf(bvh_node_t node)
{
    return left_node_idx(node) == node.first_idx;
}

bool is_right_leaf(bvh_node_t node)
{
    return right_node_idx(node) == node.last_idx;
}

enum visit_order
{
    RIGHT,
    LEFT,
    RIGHT_LEFT,
    LEFT_RIGHT,
    NONE,
    STOP
};

// returns if to continue
bool traversal_step(visit_order       order,
                    int*              stack_start,
                    int*&             stack,
                    const bvh_node_t* tree,
                    bvh_node_t&       node)
{
    switch (order)
    {

    case RIGHT_LEFT:
        *(stack++) = left_node_idx(node);
    case RIGHT:
        node = tree[right_node_idx(node)];
        return true;
    case LEFT_RIGHT:
        *(stack++) = right_node_idx(node);
    case LEFT:
        node = tree[left_node_idx(node)];
        return true;
    case NONE:
        if (stack != stack_start)
        {
            node = tree[*(--stack)];
            return true;
        }
    case STOP:
        return false;
    }
}

// precondition: tree should have a depth < 64
//               tree should habe more then 1 leaf
template <class Proc> void traverse_bvh2(const bvh_node_t* tree, Proc proc)
{

    int         stack_start[64];
    int*        stack = stack_start;
    bvh_node_t  node = tree[0];
    visit_order order;
    do
    {
        order = proc(node);
    } while (traversal_step(order, stack_start, stack, tree, node));
}
// returns the minimal distance intersection Distance
float bounds3d_intersect(bounds3d_t bound,
                         vec3       origin,
                         vec3       inverse_direction,
                         float      max_distance);

bool triangle_intersect(ray_t*  ray,
                        vec3   v1,
                        vec3   v2,
                        vec3   v3,
                        float* uv);
// IntersectF takes a max  distance, and returns the intersection distance
// If the hit is has a smaller intersection distance then the max_distance
// Else it returns the max distance
template <class IntersectF> struct closest_hit_procedure
{
    float             clip_end;
    vec3              inverse_direction;
    vec3              origin;
    const bounds3d_t* node_bounds;
    const bounds3d_t* shape_bounds;
    IntersectF        intersect_f;
    closest_hit_procedure(ray_t             ray,
                          const bvh_tree_t* tree,
                          IntersectF        inter_func)
        : clip_end(ray.clip_end), intersect_f(inter_func),
          node_bounds(tree->node_bounds), shape_bounds(tree->shape_bounds)
    {
        for (int i = 0; i < 3; i++)
            inverse_direction[i] = 1. / ray.direction[i];
        glm_vec3_copy(ray.origin, origin);
    }

    bool is_distance_valid(float distance)
    {
        return distance > 0 && distance < clip_end;
    }

    // returns the minimal intersection distance
    float intersect_child(bool is_leaf, int id)
    {
        if (is_leaf)
        {
            float distance = bounds3d_intersect(
                shape_bounds[id], origin, inverse_direction, clip_end);
            if (is_distance_valid(distance))
            {
                clip_end = intersect_f(id, clip_end);
            }
            return clip_end;
        }
        return bounds3d_intersect(
            node_bounds[id], origin, inverse_direction, clip_end);
    }
    visit_order operator()(bvh_node_t node)
    {

        bool  left_leaf = is_left_leaf(node);
        bool  right_leaf = is_right_leaf(node);
        float left_distance = intersect_child(left_leaf, left_node_idx(node));
        float right_distance
            = intersect_child(right_leaf, right_node_idx(node));
        bool right_valid = is_distance_valid(right_distance);
        bool left_valid = is_distance_valid(left_distance);
        /*
         * Truth tabel
         * left_valid right_valid left_leaf right_leaf left<right Result
         * 0          0             0           0       0
         */
        if (!right_valid && !left_valid)
            return NONE;
        if (left_leaf && right_leaf)
            return NONE;
        if (left_leaf && right_valid)
            return LEFT;
        if (right_leaf && left_valid)
            return RIGHT;

        if (right_distance < left_distance)
            return RIGHT_LEFT;
        return LEFT_RIGHT;
    }
};
