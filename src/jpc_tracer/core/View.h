#pragma once
#include <cstddef>

namespace jpctracer {
    template<class T>
    struct View
    {
        T* begin()
        {
            return data;
        }
        T* end()
        {
            return data+size;
        }
        T const * cbegin()
        {
            return data;
        }
        T const * cend()
        {
            return data+size;
        }
        T& operator[](size_t idx)
        {
            return data[idx];
        }
        T* data;
        size_t size;
    };
}