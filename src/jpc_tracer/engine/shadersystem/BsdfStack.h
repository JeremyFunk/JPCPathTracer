#pragma once
#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include <array>
#include <bits/c++config.h>
#include "IBsdfClosure.h"
#include <cassert>
#include <cstddef>

namespace jpctracer::shadersys
{

    class BsdfStack;

    struct BsdfNode
    {

    private:
        //if child, LeftNode = weight
        void* LeftNode;
        //if child, RightNode = bsdf
        void* RightNode;

        bool IsChild;
        Prec Weight;

        friend class BsdfStack;
    };

    struct BsdfMemoryState
    {
        BsdfMemoryState(std::size_t _free_space_size,void* _next_bsdf_space);
        std::size_t count = 0;
        std::size_t free_space_size;
        void* next;
    };

    class LinearCombBsdfs
    {
    public:
        LinearCombBsdfs(LinearCombBsdfs&&) = default;
        LinearCombBsdfs(const LinearCombBsdfs&) = delete;
        
        const IBsdfClosure** closures;
        const Prec* weights;
        size_t count;
        void OnDelete();
    private:
        LinearCombBsdfs(BsdfStack* stack,BsdfMemoryState state, const IBsdfClosure** _closures,const Prec* _weights,size_t _count);
        BsdfStack* m_stack;
        BsdfMemoryState m_state;
        friend BsdfStack;
    };

    //Musst live on a single thread
    class BsdfStack
    {
    private:
        static constexpr std::size_t bsdf_max_count = 1000;
        static constexpr std::size_t max_bsdfnodes_count =64 ;  
        static constexpr std::size_t full_bsdf_size = bsdf_max_count*sizeof(Spectrum);

        friend LinearCombBsdfs;

    public:

        BsdfStack() :
            m_bsdf_state(full_bsdf_size,m_bsdf_memory),m_last_bsdf_state(full_bsdf_size,m_bsdf_memory)
        {}

        template<std::derived_from<IBsdfClosure> BsdfT, 
        class... Args>
        BsdfNode* emplace(Args&&... args)
        {
            void*  bsdf = aligned_alloc_bsdf<BsdfT>();
            if(bsdf)
            {
                bsdf = new (bsdf) BsdfT{std::forward<Args>(args)...};
                
                m_weights[m_bsdf_state.count] = 0;
                m_closures[m_bsdf_state.count] = reinterpret_cast<IBsdfClosure*>(bsdf);
                

                BsdfNode& node = m_bsdf_nodes[m_bsdfnodes_count];
                node.LeftNode = &m_weights[m_bsdf_state.count];
                node.RightNode = bsdf;
                node.IsChild = true;
                node.Weight = 1;
                m_bsdfnodes_count++;
                m_bsdf_state.count++;
                return &node;
            }
            //Bsdf out of memory
            assert(bsdf!=nullptr);
            return nullptr;
            
        }

        
        
        
        LinearCombBsdfs pop_until(BsdfNode* root_node);


        
        BsdfMemoryState BsdfsState();

        BsdfNode* MixNodes(BsdfNode* node_1, BsdfNode* node_2,float value);

    private:
        void ApplyWeights(BsdfNode* node);     

        

        void ShrinkBsdfs(BsdfMemoryState state);
        void ClearBsdfNodes();

        template <std::derived_from<IBsdfClosure> T>
        T* aligned_alloc_bsdf(std::size_t alignment = alignof(T))
        {
            constexpr std::size_t size =  sizeof(T);
            if (std::align(alignment, size, m_bsdf_state.next, m_bsdf_state.free_space_size))
            {
                T* result = reinterpret_cast<T*>(m_bsdf_state.next);
                m_bsdf_state.next = (char*)m_bsdf_state.next + size;
                m_bsdf_state.free_space_size -= size;
                return result;
            }
            return nullptr;
        }

        char m_bsdf_memory[full_bsdf_size];
        std::array<Prec, bsdf_max_count> m_weights;
        std::array<BsdfNode,max_bsdfnodes_count> m_bsdf_nodes;
        std::array<const IBsdfClosure*,bsdf_max_count> m_closures;

        

        std::size_t m_bsdfnodes_count = 0;
        

        BsdfMemoryState m_bsdf_state;
        BsdfMemoryState m_last_bsdf_state;
        

    };


}