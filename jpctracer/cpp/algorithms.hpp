#pragma once

#include "search.h"
#include <algorithm>
#include <iterator>

namespace algorithms
{
template<typename I>
//I id Bidirectional iterator 
auto predecessor(I it)
{
    return --it;
}

template <typename I>
// I is ForwardIterator
void rotate_right_by_one(I first, I last, std::forward_iterator_tag)
{
    if (first == last)
        return;
    I current = first;
    while (++current != last)
        std::swap(*first, *current);
}

template <typename I>
// I is BidirectionalIterator
void rotate_right_by_one(I first, I last, std::bidirectional_iterator_tag)
{
    typedef typename std::iterator_traits<I>::value_type T;
    I                                                    butlast = last;
    --butlast;
    T x = *butlast;
    std::copy_backward(first, butlast, last);
    *first = x;
}

template <typename I> inline void rotate_right_by_one(I first, I last)
{
    rotate_right_by_one(
        first, last, typename std::iterator_traits<I>::iterator_category());
}

template <typename I, typename R>
// I is BidirectionalIterator
// R is WeakStrictOrdering on the value type of I
I linear_insert(I first, I current, R r)
{
    // precondition: is_sorted(first, current, r) && current is a valid iterator
    typedef typename std::iterator_traits<I>::value_type T;
    T                                                    value = *current;
    while (first != current && r(value, *predecessor(current)))
    {
        *current = *predecessor(current);
        --current;
    }
    *current = value;
    return current;
}

template <typename I>
// I is BidirectionalIterator
// R is WeakStrictOrdering on the value type of I
I linear_insert(I first, I current)
{
    // precondition: is_sorted(first, current, r) && current is a valid iterator
    
    using value_t = typename std::iterator_traits<I>::value_type;
    return linear_insert(
        first,
        current,
        std::less<value_t>());
}

} // namespace algorithms
