#include "traverse.hpp"
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

    bool is_leaf() const
    {
        return 0 == c.size();
    }
    auto childs_begin() const
    {
        return c.begin();
    }
    auto childs_end() const
    {
        return c.end();
    }
};
struct node_intersector
{
    using intervall_type = pair<float, float>;
    template <typename I, typename O> O operator()(I first, I last, O out) const
    {
        while (first != last)
        {
            *out = first->i;
            std::cout << "Node Intersection: [" << first->i.first << ","
                      << first->i.second << "]\n";
            first++;
            out++;
        }
        return out;
    }
};

struct leaf_intersector
{
    using intervall_type = float;

    float operator()(const intervall_node& node) const
    {
        std::cout << "Leaf Intersection: [" << node.i.first << ","
                  << node.i.second << "]\n";
        return (node.i.second + node.i.first) / 2;
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