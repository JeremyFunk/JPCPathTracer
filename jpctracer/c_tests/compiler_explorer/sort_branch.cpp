#include "../Timer.h"
#include <cassert>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <random>
#include <type_traits>
#include <vector>

template <class N, class Prec>
// N models node
// Prec float/double/...
struct closest_hit_stackitem
{
    Prec min;
    N    node;
    closest_hit_stackitem() = default;
    closest_hit_stackitem(N node, Prec min, Prec max) : node(node), min(min)
    {
    }
    auto operator<=>(const closest_hit_stackitem& x) const
    {
        return min <=> x.min;
    }
    bool operator==(const closest_hit_stackitem& x) const = default;
};

using StackI = closest_hit_stackitem<double, float>;

struct node
{
    float  data[4];
    double childs[4];

    bool operator==(const node& x) const = default;
};

int next_idx(std::uint32_t& mask)
{
    int r = __builtin_ffs(mask);
    mask &= mask - 1;
    return r - 1;
}

std::size_t bounds_intersect(const node& n, float* min, float* max)
{

    for (int i = 0; i < 4; i++)
    {
        min[i] = -n.data[i];
        max[i] = n.data[i] * 2;
    };

    std::uint32_t mask = 0;

    for (int i = 0; i < 4; i++)
    {
        int t = int(min[i] <= 0);
        mask |= t << i;
    }
    return mask;
}

template <class S> S* intersect_childs(const node& n, S* stack)
{
    float         min[4];
    float         max[4];
    std::uint32_t mask = bounds_intersect(n, min, max);

    while (mask != 0)
    {
        int i = next_idx(mask);
        // std::cout << "mask: " << mask << "\n";
        *stack = {n.childs[i], min[i], max[i]};
        stack++;
    }

    return stack;
}

template <class T> void sort1(T* first, T* last)
{
    return;
}

template <class T> constexpr void sort2(T& first, T& last)
{
    if (last > first)
    {
        auto tmp = first;
        first = last;
        last = tmp;
    }
}

template <class T> constexpr void sort3(T* first, T* last)
{
    sort2(first[0], first[1]);
    sort2(first[1], first[2]);
    sort2(first[0], first[1]);
}
template <class T> constexpr void sort4(T* first, T* last)
{
    sort2(first[1], first[0]);
    sort2(first[3], first[2]);
    sort2(first[2], first[0]);
    sort2(first[3], first[1]);
    sort2(first[2], first[1]);
}

template <class T> constexpr void sort(T* first, T* last)
{
    switch (last - first)
    {
    case 1:
        sort1(first, last);
    case 2:
        sort2(first, last);
    case 3:
        sort3(first, last);
    case 4:
        sort4(first, last);
    }
}
template <class T> constexpr void insertion_sort(T* first, T* last)
{
    T*   array = first;
    auto size = last - first;
    for (int i = 1; i < size; i++)
    {
        auto key = array[i]; // take value
        auto j = i;
        while (j > 0 && array[j - 1] > key)
        {
            array[j] = array[j - 1];
            j--;
        }
        array[j] = key; // insert in right place
    }
}

StackI* __attribute__((noinline))
closest_hit(const node& n, StackI* stack_begin)
{
    StackI* stack_last = intersect_childs(n, stack_begin);
    sort(stack_begin, stack_last);
    return stack_last;
}

StackI* __attribute__((noinline))
closest_hit3(const node& n, StackI* stack_begin)
{

    StackI* stack_last = intersect_childs(n, stack_begin);
    insertion_sort(stack_begin, stack_last);
    return stack_last;
}

StackI* __attribute__((noinline)) closest_hit2(const node& n, StackI* stack)
{
    float min[4];
    float max[4];

    std::uint32_t mask = bounds_intersect(n, min, max);

    int  i;
    auto stack_b = stack;
    if (mask == 0)
        goto ret;
    i = next_idx(mask);
    *stack = {n.childs[i], min[i], max[i]};
    stack++;
    if (mask == 0)
    {
        sort1(stack_b, stack);
        goto ret;
    }

    i = next_idx(mask);
    *stack = {n.childs[i], min[i], max[i]};
    stack++;
    if (mask == 0)
    {
        sort2(stack_b, stack);
        goto ret;
    }
    i = next_idx(mask);
    *stack = {n.childs[i], min[i], max[i]};
    stack++;
    if (mask == 0)
    {
        sort3(stack_b, stack);
        goto ret;
    }
    i = next_idx(mask);
    *stack = {n.childs[i], min[i], max[i]};
    stack++;

    sort4(stack_b, stack);

ret:
    return stack;
}

void compare(StackI* stack1, StackI* end1, StackI* stack2)
{
    while (stack1 != end1)
    {
        assert(*stack1 == *stack2);
        stack1++;
        stack2++;
    }
}

int main()
{
    int                n = 1e8;
    std::vector<node>  nodes(n);
    std::random_device r;

    // Choose a random mean between -1000 and 1000
    std::default_random_engine            e1(r());
    std::uniform_real_distribution<float> uniform_float(-1000, 1000);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < 4; j++)
            nodes[i].childs[j] = uniform_float(e1);
    }

    StackI stack[4];
    auto   stack_end = closest_hit(nodes[0], stack);

    StackI stack2[4];
    auto   stack2_end = closest_hit2(nodes[0], stack2);

    StackI stack3[4];
    auto   stack3_end = closest_hit3(nodes[0], stack3);

    compare(stack, stack_end, stack2);
    compare(stack2, stack2_end, stack3);

    timer  t;
    double time;
    t.start();
    for (const auto& n : nodes)
    {
        closest_hit(n, stack);
    }
    time = t.stop();

    std::cout << "Closest hit 1: " << time << "\n";

    t.start();
    for (const auto& n : nodes)
    {
        closest_hit2(n, stack);
    }
    time = t.stop();

    std::cout << "Closest hit 2: " << time << "\n";

    t.start();
    for (const auto& n : nodes)
    {
        closest_hit3(n, stack);
    }
    time = t.stop();

    std::cout << "Closest hit 3: " << time << "\n";
}