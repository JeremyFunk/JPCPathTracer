#include "StackParamsAllocator.h"
#include "jpc_tracer/raytracing/Scene.h"
#include <cassert>
#include <cstddef>
#include <memory>



namespace jpctracer::raytracing
{
    StackParamsAllocator::StackParamsAllocator(int max_stack_count, size_t max_params_size) 
    : m_free_sizes(max_stack_count+1), m_free_spaces(max_stack_count+1), m_max_stack_size(max_stack_count)
    {

        m_free_sizes[0] = max_params_size*max_stack_count*sizeof(std::byte);
        m_free_spaces[0] = malloc(m_free_sizes[0]);
    }

    void* StackParamsAllocator::Reallocate(AllocationData alloc_data) 
    {
        void*& previous_space = m_free_spaces[m_stack_index-1];
        void*& next_space = m_free_spaces[m_stack_index];
        size_t& previous_size = m_free_sizes[m_stack_index-1];
        size_t& next_size = m_free_sizes[m_stack_index];

        next_size = previous_size;
        next_space = previous_space;

        void* mem = std::align(alloc_data.Alignment,alloc_data.Size,next_space,next_size);

        assert(mem != nullptr);

        next_space = ((std::byte*)mem)+alloc_data.Size;
        next_size -= alloc_data.Size;
        return mem;
    }
    
    void StackParamsAllocator::IncrementStack() 
    {
        m_stack_index++;
        assert(m_stack_index<=m_max_stack_size);
        assert(m_stack_index>0);
    }
    
    void StackParamsAllocator::DecrementStack() 
    {
        m_stack_index--;
        assert(m_stack_index<=m_max_stack_size);
        assert(m_stack_index>0);
    }



}