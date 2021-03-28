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

namespace jpctracer
{

    class BsdfPool;

    struct BsdfNode
    {

    private:
        //if child, LeftNode = weight
        void* LeftNode;
        //if child, RightNode = bsdf
        void* RightNode;

        bool IsChild;
        Prec Weight;

        friend class BsdfPool;
    };

    struct BsdfMemoryState
    {
        BsdfMemoryState(std::size_t _free_space_size,void* _next_bsdf_space);
        std::size_t count = 0;
        std::size_t free_space_size;
        void* next;
    };

    struct LinearCombBsdfs
    {
        const IBsdfClosure** closures;
        const Prec* weights;
        size_t count;
    };

    struct BsdfFactoryReseter;

    //Musst live on a single thread
    class BsdfPool
    {
    private:
        static constexpr std::size_t bsdf_max_count = 1000;
        static constexpr std::size_t max_bsdfnodes_count =64 ;  
        static constexpr std::size_t full_bsdf_size = bsdf_max_count*sizeof(Spectrum);

    public:

        BsdfPool() :
            m_bsdf_state(full_bsdf_size,m_bsdf_memory)
        {}

        template<std::derived_from<IBsdfClosure> BsdfT, 
        class... Args>
        BsdfNode* Create(Args&&... args)
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

        
        void ClearBsdfNodes();
        
        LinearCombBsdfs GetBsdfs(BsdfMemoryState start_state);


        void ApplyWeights(BsdfNode* node);     

        BsdfMemoryState BsdfsState();

        void ShrinkBsdfs(BsdfMemoryState state);

        BsdfNode* MixNodes(BsdfNode* node_1, BsdfNode* node_2,float value);

    private:

        
        

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

    };


    class BsdfFactorySaver
    {
    public:
        BsdfFactorySaver(BsdfPool* factory);
        void Restore();
    private:
        BsdfMemoryState m_bsdf_state;
        BsdfPool* m_factory;
    };

    //---------
    /*
    
    */
}