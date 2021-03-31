#include "BsdfStack.h"
#include <bits/c++config.h>
#include <cassert>
#include <memory>
#include <iostream>


namespace jpctracer
{
    BsdfMemoryState::BsdfMemoryState(std::size_t _free_space_size,void* _next_bsdf_space) 
        :free_space_size(_free_space_size),next(_next_bsdf_space)
    {
        
    }
    
    LinearCombBsdfs::LinearCombBsdfs(BsdfStack* stack,BsdfMemoryState state, const IBsdfClosure** _closures,const Prec* _weights,size_t _count) 
        : m_stack(stack),m_state(state),closures(_closures),weights(_weights),count(_count)
    {
        
    }

    
    void LinearCombBsdfs::OnDelete() 
    {
        //Check if all linearCombs, created later then this linearcomb, where destroyed 
        //assert(m_state.count+count == m_stack->m_bsdf_state.count);
        m_stack->ShrinkBsdfs(m_state);
    }
    
    LinearCombBsdfs BsdfStack::pop_until(BsdfNode* root_node) 
    {
        ApplyWeights(root_node);
        ClearBsdfNodes();
        std::size_t bsdf_start_idx = m_last_bsdf_state.count;
        const IBsdfClosure** bsdf_start = &m_closures[bsdf_start_idx];
        const Prec* weights_start = &m_weights[bsdf_start_idx];
        size_t bsdf_count = m_bsdf_state.count-bsdf_start_idx;
        BsdfMemoryState state = m_last_bsdf_state;
        m_last_bsdf_state = m_bsdf_state;
        return {this,state,bsdf_start,weights_start,bsdf_count};
    }
    
    BsdfMemoryState BsdfStack::BsdfsState() 
    {
        return m_bsdf_state;
    }

    
    void BsdfStack::ShrinkBsdfs(BsdfMemoryState state) 
    {
        if(state.count<m_bsdf_state.count)
            m_bsdf_state = state;
    }
    
    BsdfNode* BsdfStack::MixNodes(BsdfNode* node_1, BsdfNode* node_2,float value) 
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
    
    
    
    void BsdfStack::ClearBsdfNodes() 
    {
        m_bsdfnodes_count = 0;
    }



    

    void BsdfStack::ApplyWeights(BsdfNode* node) 
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