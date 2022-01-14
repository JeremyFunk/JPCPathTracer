#pragma once
#include <stddef.h>
#include <stdint.h>
typedef struct
{
    void*    memory;
    uint64_t used, size;
} stack_allocator_t;

void* stack_alloc(stack_allocator_t* allocator,
                  uint64_t           size,
                  uint64_t           alignment);
