#include "allocators.h"
#include <assert.h>
void* scratch_alloc(scratch_allocator_t* allocator, uint64_t size, uint64_t alignment)
{
    uint8_t* mem = ((uint8_t*)allocator->memory) + allocator->used;
    uint8_t* aligned = (uint8_t*)((((uint64_t)mem) + alignment - 1) & ~(alignment - 1));
    uint64_t offset = aligned - mem;
    size += offset;

    assert(allocator->used + size <= allocator->size);
    allocator->used += size;

    return aligned;
}
