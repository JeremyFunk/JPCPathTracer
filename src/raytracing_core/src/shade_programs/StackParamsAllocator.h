#pragma once

#include "raytracing_core/include/Scene.h"
#include <cstddef>
#include <vector>
namespace jpc_rt_core
{
    
    class StackParamsAllocator
    {
    public:
        StackParamsAllocator(int max_stack_count, size_t max_params_size);

        void* Reallocate(AllocationData alloc_data);

        void IncrementStack();
        void DecrementStack();

    private:
        std::vector<void*> m_free_spaces;
        std::vector<size_t> m_free_sizes;
        int m_stack_index = 1;
        int m_max_stack_size = -1;

    };
}