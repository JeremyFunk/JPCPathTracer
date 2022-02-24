#include "bvh_old/intersect.hpp"
#include "bvh_old/lbvh.hpp"
#include "bvh_old/traverse.hpp"
#include "simdpp/simd.h"
#include <array>
#include <cstdio>
#include <iostream>
#include <limits>
#include <vector>

struct intervall_node
{
    const static std::size_t max_childs_count = 3;
    pair<float, float>       i; // intervall

    std::vector<intervall_node> c; // childs
};
bool is_leaf(const intervall_node& node)
{
    return 0 == node.c.size();
}

struct node_intersector
{

    template <typename Intervall, typename O>
    O operator()(const intervall_node& node,
                 const Intervall&      intervall,
                 O                     out) const
    {

        std::cout << "Node Intersection: [" << node.i.first << ","
                  << node.i.second << "]\n";
        for (const auto& child : node.c)
        {
            std::cout << "     Child Intersection: [" << child.i.first << ","
                      << child.i.second << "]\n";
            if (does_intersect(child.i, intervall))
            {
                *out = {child, t_min(child.i), t_max(child.i)};
                out++;
            }
        }
        return out;
    }
};

struct leaf_intersector
{
    using intervall_type = float;
    template <class Intervall>
    float operator()(const intervall_node& node,
                     Intervall             intervall,
                     intervall_type        init) const
    {
        std::cout << "Leaf Intersection: [" << node.i.first << ","
                  << node.i.second << "]\n";
        float tmp = (node.i.second + node.i.first) / 2;
        if (does_intersect(tmp, intervall))
        {
            return tmp;
        }
        return init;
    }
};

int main()
{
    // clang-format off
    //level_1
    std::vector<intervall_node> l1 = 
    {{.i={-1,-2}} ,{.i={2,4}}         ,{.i={2,3}} ,{.i={7,9}},{.i={3,8}}};
    
    std::vector<intervall_node> l2 = //level_2
    {      {.i={-2,4},.c={l1[0],l1[1]}},            {.i={2,9},.c={l1[2],l1[3],l1[4]}}};

    intervall_node root = 
                                {.i={-2,9},.c={l2[1],l2[0]}};
    // clang-format on

    std::vector<pair<float, float>> test_intervalle
        = {{3, 10}, {-4, 10}, {8.6, 20}, {1.7, 9}, {4, 10}};
    for (auto intervall : test_intervalle)
    {
        float result
            = closest_intersect(root,
                                node_intersector{},
                                leaf_intersector{},
                                intervall,
                                std::numeric_limits<float>::infinity());

        std::cout << "Intervall test [" << intervall.first << ","
                  << intervall.second << "] : result: " << result << "\n";
    }
    std::cout << "new tree\n";
    intervall_node root2 = {.i = {-2, 9}};
    float          result = closest_intersect(root2,
                                     node_intersector{},
                                     leaf_intersector{},
                                     pair<float, float>{-5, 15},
                                     std::numeric_limits<float>::infinity());
    std::cout << "Intervall test [" << -5 << "," << 15
              << "] : result: " << result << "\n";
}