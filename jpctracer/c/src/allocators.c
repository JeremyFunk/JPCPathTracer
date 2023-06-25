#include "allocators.h"
#include <stdio.h>
#include <assert.h>
#include <malloc.h>


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

int scratch_check(scratch_allocator_t* allocator, uint64_t size,uint64_t alignment)
{

    uint8_t* mem = ((uint8_t*)allocator->memory) + allocator->used;
    uint8_t* aligned = (uint8_t*)((((uint64_t)mem) + alignment - 1) & ~(alignment - 1));
    uint64_t offset = aligned - mem;
    size += offset;

    return allocator->used + size <= allocator->size;
}


#ifndef DEBUG_ARENA



typedef struct arena_block_s{
    scratch_allocator_t allocator;
    struct arena_block_s* next;
}arena_block_t;

typedef struct arena_s{
    arena_block_t* start;
    arena_block_t* current_free;
    //For information;
    uint64_t all_size;
}arena_t;

arena_t* arena_make()
{
    arena_t* arena = (arena_t*) malloc(sizeof(arena_t));
    arena->current_free = NULL;
    arena->start = NULL;

    return arena;
    
}


arena_block_t* arena_create_block(size_t size)
{
    if(size < ARENA_MIN_BLOCK_SIZE)
    {
        size =ARENA_MIN_BLOCK_SIZE;
    }
    uint8_t* mem = (uint8_t*) malloc(sizeof(arena_block_t)+size);

    arena_block_t* block = (arena_block_t*) mem;

    block->allocator.memory = (void*) (mem+sizeof(arena_block_t));
    block->allocator.size = size;
    block->allocator.used = 0;
    block->next = NULL;
    return block;

}


void arena_reserve_aligned(arena_t* arena,size_t size,size_t alignment)
{
    if(!arena->start)
    {
        arena->start = arena_create_block(size+alignment);         
        arena->current_free = arena->start;
        return;
    }
    assert(arena->current_free);

    if(!scratch_check(&arena->current_free->allocator,size,alignment))
    {
        arena->current_free->next = arena_create_block(size+alignment);
        arena->current_free = arena->current_free->next;
    }
    
}


void arena_reserve(arena_t* arena, size_t size)
{
    arena_reserve_aligned(arena,size,_Alignof(char));
}

void* arena_alloc(arena_t* arena, size_t size)
{
    return arena_alloc_aligned(arena,size,_Alignof(char));
}
void* arena_alloc_aligned(arena_t* arena, size_t size,size_t aligment)
{
    arena_reserve_aligned(arena,size,aligment);
    return scratch_alloc(&arena->current_free->allocator,size,aligment);
}
void arena_release(arena_t* arena)
{
    arena_block_t* next_block = arena->start;
    while(next_block)
    {
        arena_block_t* curr_block = next_block;
        next_block = curr_block->next;
        free(curr_block);
    }

    free(arena);
}


size_t arena_memory_used(const arena_t* arena)
{
    size_t mem = 0;
   for(arena_block_t* block = arena->start; block; block= block->next ) 
   {
        mem+=block->allocator.used;
   }
   return mem;
}


size_t arena_memory_allocated(const arena_t* arena)
{

    size_t cap = 0;
   for(arena_block_t* block = arena->start; block; block= block->next ) 
   {
        cap+=block->allocator.size;
   }
   return cap;
}

#else


typedef struct arena_s{
    void** allocs;
    uint size;
    size_t alloced_size;
}arena_t;


arena_t* arena_make()
{
    arena_t* arena = malloc(sizeof(arena_t));
    arena->allocs = NULL;
    arena->size= 0;
    arena->alloced_size = 0;
    return arena;
}
void arena_reserve(arena_t* arena,size_t size){

}
void* arena_alloc(arena_t* arena, size_t size)
{
    return arena_alloc_aligned(arena,size,_Alignof(char));
}
void* arena_alloc_aligned(arena_t* arena, size_t size,size_t aligment)
{
    arena->allocs = realloc(arena->allocs,(arena->size+1)*sizeof(void*));
    arena->allocs[arena->size] = aligned_alloc(aligment,size);
    arena->size++;
    arena->alloced_size += size;
    return arena->allocs[arena->size-1];
}
void arena_release(const arena_t* arena)
{
    for(uint i = 0; i<arena->size;i++)
    {
        aligned_free(arena->allocs[i]);
    }
    free(arena);
}

size_t arena_memory_used(const arena_t* arena)
{
    return arena->alloced_size;
}

#endif

void arena_print_stats(arena_t* arena)
{
    printf("Arena memory used: %zu\n", arena_memory_used(arena));
    printf("Arena memory allocated: %zu\n", arena_memory_allocated(arena));


}