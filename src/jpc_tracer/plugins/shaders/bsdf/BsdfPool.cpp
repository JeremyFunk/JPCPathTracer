#include "BsdfPool.h"
#include <memory>
#include <iostream>


namespace jpctracer
{
    BsdfMemoryState::BsdfMemoryState(std::size_t _free_space_size,void* _next_bsdf_space) 
        :free_space_size(_free_space_size),next(_next_bsdf_space)
    {
        
    }
    
    BsdfMemoryState BsdfPool::BsdfsState() 
    {
        return m_bsdf_state;
    }

    
    void BsdfPool::ShrinkBsdfs(BsdfMemoryState state) 
    {
        m_bsdf_state = state;
    }
    
    BsdfNode* BsdfPool::MixNodes(BsdfNode* node_1, BsdfNode* node_2,float value) 
    {
        
        BsdfNode* result = &m_bsdf_nodes[m_bsdfnodes_count];
        m_bsdfnodes_count++;

        result->LeftNode = node_1;
        result->RightNode = node_2;
        if(node_1 != nullptr)
            node_1->Weight *=value;
        if(node_2 != nullptr)
            node_2->Weight *= 1-value;
        result->Weight = 1;
        result->IsChild = false;
        return result;
    }
    
    BsdfFactorySaver::BsdfFactorySaver(BsdfPool* factory) 
    :m_bsdf_state(factory->BsdfsState()), m_factory(factory)
    {
    }
    
    void BsdfFactorySaver::Restore() 
    {
        m_factory->ShrinkBsdfs(m_bsdf_state);
    }
    
    void BsdfPool::ClearBsdfNodes() 
    {
        m_bsdfnodes_count = 0;
    }

    
    LinearCombBsdfs BsdfPool::GetBsdfs(BsdfMemoryState start_state) 
    {
        const IBsdfClosure** bsdf_start = &m_closures[start_state.count];
        const Prec* weights_start = &m_weights[start_state.count];
        size_t bsdf_count = m_bsdf_state.count-start_state.count;
        return LinearCombBsdfs{bsdf_start,weights_start,bsdf_count};
    }


    

    void BsdfPool::ApplyWeights(BsdfNode* node) 
    {
        constexpr size_t max_stack = bsdf_max_count;

        BsdfNode* stack[max_bsdfnodes_count];
        BsdfNode** stack_begin = stack-1;
        BsdfNode** stack_it = stack;

        *stack_it = node;

        for(;stack_it!=stack_begin;stack_it--)
        {
            BsdfNode* node = *stack_it;

            if(node->IsChild)
            {
                Prec* weight = reinterpret_cast<Prec*>(node->LeftNode);

                *weight = node->Weight;
                continue;
            }
            BsdfNode* left_node = reinterpret_cast<BsdfNode*>(node->LeftNode);
            if(left_node!=nullptr)
            {
                left_node->Weight*=node->Weight;
                *stack_it = left_node;
                stack_it++;
            }
            BsdfNode* right_node = reinterpret_cast<BsdfNode*>(node->RightNode);
            if(left_node!=nullptr)
            {
                right_node->Weight*=node->Weight;
                *stack_it = right_node;
                stack_it++;
            }            

            
        }

    }


    
}