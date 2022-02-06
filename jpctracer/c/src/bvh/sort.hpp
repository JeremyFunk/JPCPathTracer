
#pragma once
#include <algorithm>
template <class T>

// T: semiregular
inline void sort2(T first, T second)
{
    if (*second < *first) // last < first
    {
        auto tmp = *first;
        *first = *second;
        *second = tmp;
    }
}

template <class T>
// T: Random access iterator with a semiregular valuetype
inline constexpr void sort3(T first) noexcept
{
    sort2(first + 0, first + 1);
    sort2(first + 1, first + 2);
    sort2(first + 0, first + 1);
}

template <class T>
// T: Random access iterator with a semiregular valuetype
inline constexpr void sort4(T first) noexcept
{
    sort2(first + 1, first + 0);
    sort2(first + 3, first + 2);
    sort2(first + 2, first + 0);
    sort2(first + 3, first + 1);
    sort2(first + 2, first + 1);
}

template <class T> inline constexpr void small_sort(T first, T last)
{
    switch (last - first)
    {
    case 2:
        return sort2(first, last - 1);
    case 3:
        return sort3(first);
    case 4:
        return sort4(first);
    default:
        return std::sort(first, last);
    }
}