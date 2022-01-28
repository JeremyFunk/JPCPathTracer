#pragma once
#include "sort.hpp"
#include <limits>
extern "C"
{
#include "bvh.h"
#include "jpc_api.h"
}
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <stack>
#include <type_traits>
#include <utility>
#include <vector>

template <typename Intersect> struct child_intersector_sorted
{
    Intersect intersect;
    child_intersector_sorted(const Intersect& intersect) : intersect(intersect)
    {
    }

    template <class N, typename O, typename Intervall>
    // N tree cooridnate
    // node should not be a leaf
    // O output_iterator *out={node,float,float} and randomaccessiterator
    O operator()(const N& node, const Intervall& intervall, O out_start) const
    {
        O out_end = intersect(node, intervall, out_start);
        small_sort(out_start, out_end);
        return out_end;
    }
};

template <typename N, typename S, typename Intersector, typename Intervall>
// pushes all the childs not visited on the stack
// S bidirection iterator
N find_leaf(N                  node,
            S&                 stack,
            const Intersector& intersect,
            const Intervall&   intervall)
{
    S stack_begin = stack;
    while (!is_leaf(node))
    {

        stack = intersect(node, intervall, stack);
        if (stack != stack_begin)
        {
            stack--;
            node = stack->node;
        }
        else
        {
            break;
        }
    };
    return node;
}

template <typename N, typename Prec>
// N node
// Prec Floatingpoint type
struct closest_hit_stackitem
{
    N    node;
    Prec min;
    closest_hit_stackitem() = default;
    closest_hit_stackitem(const N& node, const Prec& _min, const Prec& _max)
        : node(node), min(_min)
    {
    }
    auto operator<=>(const closest_hit_stackitem& x) const
    {
        return x.min <=> min;
    }
};

template <class N,
          class NodeIntersector,
          class LeafIntersector,
          class Intervall>
// N models tree iterator
// NodeIntersector::intervall_type* =
// NodeIntersector(node.childs_begin(),node.childs_end(),NodeIntersector::intervall_type*
// out);
typename LeafIntersector::intervall_type closest_intersect(
    N                                        node,
    const NodeIntersector&                   node_intersect,
    const LeafIntersector&                   leaf_intersect,
    Intervall                                search_intervall,
    typename LeafIntersector::intervall_type init)
{
    using leaf_intervall_type = typename LeafIntersector::intervall_type;
    using prec =
        typename std::remove_reference<decltype(t_min(search_intervall))>::type;
    using stack_type = closest_hit_stackitem<N, prec>;

    stack_type stack[64];

    leaf_intervall_type                       leaf_intervall = init;
    child_intersector_sorted<NodeIntersector> node_intersect_sorted(
        node_intersect);

    stack_type* stack_start = stack;
    stack_type* stack_it = stack;

    *stack_it = {node,
                 t_min(search_intervall) + prec(ERROR_THICKNESS),
                 std::numeric_limits<prec>::infinity()};
    stack_it++;
    while (stack_it != stack_start)
    {
        stack_it--;
        prec min_dist = stack_it->min;
        node = stack_it->node;
        if (min_dist < t_max(search_intervall))
        // max_dist > t_min(search_intervall) already tested
        {
            node = find_leaf(
                node, stack_it, node_intersect_sorted, search_intervall);
            if (is_leaf(node))
            {
                leaf_intervall
                    = leaf_intersect(node, search_intervall, leaf_intervall);
                t_max(search_intervall)
                    = std::min(t_min(leaf_intervall), t_max(search_intervall));
            }
        }
    }
    return leaf_intervall;
}