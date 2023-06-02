#pragma once
#include <utility>
extern "C"
{
#include "allocators.h"
}
template <class T> T* scratch_alloc(scratch_allocator_t* allocator)
{
    return (T*)scratch_alloc(allocator, sizeof(T), alignof(T));
}
template <class T>
T* scratch_alloc_n(scratch_allocator_t* allocator, uint64_t count)
{
    return (T*)scratch_alloc(allocator, sizeof(T) * count, alignof(T));
}
template <class T, class... Args>
T* scratch_alloc(scratch_allocator_t* allocator, Args&&... args)
{
    void* data = scratch_alloc<T>(allocator);
    return new (data) T{std::forward<Args>(args)...};
}