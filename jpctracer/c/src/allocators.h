#pragma once
#include <stddef.h>
#include <stdint.h>
#include <jpc_api.h>
#include <malloc.h>

#define ARENA_MIN_BLOCK_SIZE 1024

typedef struct
{
    void*    memory;
    uint64_t used, size;
} scratch_allocator_t;

void* scratch_alloc(scratch_allocator_t* allocator,
                  uint64_t           size,
                  uint64_t           alignment);

int scratch_check(scratch_allocator_t* allocator, uint64_t size,uint64_t alignment);


#ifdef _MSC_VER
inline void* aligned_alloc(size_t aligment, size_t size)
{
    return _aligned_malloc(size, aligment);
}

inline void aligned_free(void* ptr)
{
    _aligned_free(ptr);
}

#else
inline void aligned_free(void* ptr)
{
    free(ptr);
}
#endif
