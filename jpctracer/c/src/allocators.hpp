#pragma once
#include <utility>
extern "C"
{
#include "allocators.h"
}
template <class T> T* stack_alloc(stack_allocator_t* allocator)
{
    return (T*)stack_alloc(allocator, sizeof(T), alignof(T));
}
template <class T>
T* stack_alloc_n(stack_allocator_t* allocator, uint64_t count)
{
    return (T*)stack_alloc(allocator, sizeof(T) * count, alignof(T));
}
template <class T, class... Args>
T* stack_alloc(stack_allocator_t* allocator, Args&&... args)
{
    void* data = stack_alloc<T>(allocator);
    return new (data) T{std::forward<Args>(args)...};
}