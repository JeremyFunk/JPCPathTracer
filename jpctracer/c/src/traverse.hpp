#pragma once
extern "C"
{
#include "bvh.h"
#include "jpc_api.h"
}
#include "intersect.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <stack>
#include <type_traits>
#include <utility>
#include <vector>

template <typename I1, typename I2, typename O, typename UnPred>
// I models Iterator with Value type
// updates to next node
// O models OutputIterator with Valuetype std::pair<Valuetype(I1),Valuetype(I2)>
// Pred models binary predicate on Valuetype of I1
O combine_if(I1 first1, I1 last1, I2 first2, O first_out, UnPred pred)
{
    while (first1 != last1)
    {
        if (pred(*first1, *first2))
        {
            *first_out = pair{*first1, *first2};
            first_out++;
        }
        first1++;
        first2++;
    }
    return first_out;
}
template <typename I1, typename I2, typename O, typename UnPred, typename Op>
// I models Iterator with Value type
// updates to next node
// O models OutputIterator with Valuetype std::pair<Valuetype(I1),Valuetype(I2)>
// Pred models binary predicate on Valuetype of I1
O transform_if(I1 first1, I1 last1, I2 first2, O first_out, Op op, UnPred pred)
{
    while (first1 != last1)
    {
        if (pred(*first1, *first2))
        {
            *first_out = op(*first1, *first2);
            first_out++;
        }
        first1++;
        first2++;
    }
    return first_out;
}
template <std::size_t N, typename I, typename Compare>
// I models random access iterator
// N is the max distance between first and last
// Compare striktlyweakordering
void small_sort(I first, I last, Compare cmp)
{
    if (N == 2)
    {
        small_sort2(first, last, cmp);
    }
    else
    {
        std::sort(first, last, cmp); // should be insertion sort
    }
}

template <typename I, typename Compare>
void small_sort2(I first, I last, Compare cmp)
{
    last--;
    if (first == last)
        return;
    if (cmp(*last, *first)) //*first > *last
        std::swap(*first, *last);
}
inline int left_node_idx(bvh_node_t node)
{
    return node.split_idx;
}

inline int right_node_idx(bvh_node_t node)
{
    return node.split_idx + 1;
}

inline bool is_left_leaf(bvh_node_t node)
{
    return left_node_idx(node) == node.first_idx;
}

inline bool is_right_leaf(bvh_node_t node)
{
    return right_node_idx(node) == node.last_idx;
}

struct lbvh_iterator
{
    bvh_tree_t* tree;
    int         idx;

    bool leaf;

    bool is_leaf() const
    {
        return leaf;
    }
    // forward iterator
    struct child_iterator
    {
        const static std::uint16_t THIS_RIGHT = 3; // 11
        const static std::uint16_t THIS = 1;       // 01
        const static std::uint16_t RIGHT = 2;      // 10
        const static std::uint16_t NO = 0;         // 00

        const static std::size_t max_childs_count = 2;

        bvh_tree_t* tree;
        int         idx;

        std::uint16_t flags;

        child_iterator& operator++()
        {
            idx++;
            flags = flags >> 1;
            return *this;
        }
        child_iterator operator++(int)
        {
            child_iterator res = *this;
                           operator++();
            return res;
        }

        int operator-(const lbvh_iterator& a) const
        {
            return idx - a.idx;
        }
        bool operator==(const lbvh_iterator& a) const
        {
            return idx == a.idx;
        }
        lbvh_iterator operator*()
        {
            return lbvh_iterator{tree, idx, (flags | THIS) == 0};
        }
    };
    child_iterator childs_begin() const
    {
        bvh_node_t    node = tree->nodes[idx];
        bool          left_leaf = is_left_leaf(node);
        bool          right_leaf = is_right_leaf(node);
        std::uint16_t leaf_order = child_iterator::NO;
        if (is_left_leaf(node))
            leaf_order |= child_iterator::THIS;
        if (is_right_leaf(node))
            leaf_order |= child_iterator::RIGHT;

        return child_iterator{tree, left_node_idx(node), leaf_order};
    }
    child_iterator childs_end() const
    {

        bvh_node_t node = tree->nodes[idx];
        return child_iterator{
            tree, right_node_idx(node) + 1, child_iterator::NO};
    }
};

template <class N,
          typename O,
          typename Intersect,
          typename Intervall,
          typename Op>
// N tree cooridnate
// node should not be a leaf
// O output_iterator *out=store(Intersect::intervall_type,node)
// store stores the element in the output iterator
O intersect_childs(const N&         node,
                   O                out,
                   const Intersect& intersect,
                   const Intervall& intervall,
                   Op               store)
{
    using intervall_type = typename Intersect::intervall_type;

    intervall_type child_intervalls[N::max_childs_count];

    intervall_type* intervall_first = child_intervalls;

    auto childs_first = node.childs_begin();
    auto childs_last = node.childs_end();

    auto intervall_last = intersect(childs_first, childs_last, intervall_first);
    return transform_if(intervall_first,
                        intervall_last,
                        childs_first,
                        out,
                        store,
                        [&](const intervall_type& x, const N& child) {
                            return does_intersect(x, intervall);
                        });
}

template <class N, class S, typename Intersector, typename Intervall>
// pushes all the childs not visited on the stack
// S bidirection iterator
N find_closest_leaf(N                  node,
                    S&                 stack,
                    const Intersector& intersect,
                    const Intervall&   intervall)
{
    using prec = decltype((*stack).first);
    S stack_begin = stack;
    while (!node.is_leaf())
    {

        S tmp_stack = intersect_childs(
            node,
            stack,
            intersect,
            intervall,
            [](const auto& _intervall, const N& node) {
                return pair<prec, N>{t_min(_intervall), node};
            });
        if (tmp_stack != stack)
        {
            small_sort<N::max_childs_count>(
                stack, tmp_stack, [](const auto& a, const auto& b) {
                    return a.first > b.first;
                });
            stack = tmp_stack;
        };
        if (stack != stack_begin)
        {
            stack--;
            node = (*stack).second;
        }
        else
        {
            break;
        }
    };
    return node;
}

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
    using node_intervall_type = typename NodeIntersector::intervall_type;
    using leaf_intervall_type = typename LeafIntersector::intervall_type;
    using prec =
        typename std::remove_reference<decltype(t_min(search_intervall))>::type;
    using stack_type = pair<prec, N>;

    stack_type stack[64];

    leaf_intervall_type leaf_intervall = init;

    stack_type* stack_start = stack;
    stack_type* stack_it = stack;

    *stack_it
        = pair<float, N>{t_min(search_intervall) + prec(ERROR_THICKNESS), node};
    stack_it++;
    while (stack_it != stack_start)
    {
        stack_it--;
        prec min_dist = (*stack_it).first;
        node = (*stack_it).second;
        if (min_dist < t_max(search_intervall))
        // max_dist > t_min(search_intervall) already tested
        {
            node = find_closest_leaf(
                node, stack_it, node_intersect, search_intervall);
            if (node.is_leaf())
            {
                leaf_intervall_type tmp = leaf_intersect(node);
                if (does_intersect(tmp, search_intervall))
                {
                    leaf_intervall = tmp;
                    t_max(search_intervall) = t_min(leaf_intervall);
                }
            }
        }
    }
    return leaf_intervall;
}