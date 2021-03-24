#pragma once
#include "../Concepts.h"
#include "../cache/ShaderCache.h"
#include "jpc_tracer/core/maths/Constants.h"
#include <tuple>
#include <type_traits>
#include <utility>
#include "../utility.h"
#include "DebugShader.h"

namespace jpctracer::shader::detail {
    
    
    
    template<class L, class R, class V>
    struct MixerNode
    {
        MixerNode(L l, R r, V v): left(l), right(r), influence(v) {}
        L left;
        R right;
        V influence;
    };
    //is_mixernode

    template<class T>
    struct is_mixernode
    {
        static constexpr bool value = false;
    };

    template<class L, class R, class V>
    struct is_mixernode<MixerNode<L,R,V>>
    {
        static constexpr bool value = true;
    };

    //FlattenNodes
    //**************************************************************************

    template<MaterialType mat_type,class T>
    struct FlattenNodes
    {
        using type = std::tuple<>;
    };

    template<MaterialType mat_type, __BsdfCreator<mat_type> T>
    struct FlattenNodes<mat_type,T>
    {
        
        using inner_type = decltype(((T*) 0 )->template CreateDistribution<mat_type>(SharedMemory{}));
        using type = std::tuple<inner_type>;
    };


    template<MaterialType mat_type,class L,class R,class V>
    struct FlattenNodes<mat_type,MixerNode<L,R,V>>
    {
        using left = typename FlattenNodes<mat_type,L>::type;
        using right = typename FlattenNodes<mat_type,R>::type;
        using type = typename combine_tuple_traits<left, right>::type;
    };

    //SetupMixer
    //**************************************************************************

    template<MaterialType type, size_t idx,__BsdfCreator<type> T, class Tuple,
            class Array>
    void SetupMixer(const T& creator,Tuple& tuple, Array& array,
                    const SharedMemory& memory)
    {
        std::get<idx>(tuple) = creator.template CreateDistribution<type>(memory);
    }

    template<MaterialType type, size_t idx,class T, class Tuple,
            class Array>
    void SetupMixer(const T& creator,Tuple& tuple, Array& array,
                    const SharedMemory& memory)
    {
    }

    template<MaterialType type, size_t idx, class L,class R,class V,class Tuple,
            class Array>
    void SetupMixer(const MixerNode<L, R, V>& creator,Tuple& tuple, 
                    Array& array,  const SharedMemory& memory)
    {
        using left_pack = typename FlattenNodes<type,L>::type;
        using right_pack = typename FlattenNodes<type,R>::type;

        constexpr size_t idx_left = idx;
        constexpr size_t left_length = std::tuple_size<left_pack>::value;
        constexpr size_t idx_right = idx+left_length;
        constexpr size_t right_length = std::tuple_size<right_pack>::value;



        if(left_length>0)
            SetupMixer<type,idx_left>(creator.left,tuple,array,memory);
        if(right_length>0)
            SetupMixer<type,idx_right>(creator.right,tuple,array,memory);

        Prec w = Eval(creator.influence,memory.Interaction);

        for(int i= idx_left; i<idx_right;i++)
            array[i]*=w;
        for(int i=idx_right;i<idx_right+right_length;i++)
            array[i]*=1-w;
    }

    //**************************************************************************

    template<class First,class Second, ValueBuilder Influence>
    class BsdfMixerBuilder;

    template<class T>
    struct is_bsdf_mixer_builder
    {
        static constexpr bool value = false;
    };

    template<class F,class S,class I>
    struct is_bsdf_mixer_builder<BsdfMixerBuilder<F, S, I>>
    {
        static constexpr bool value = true;
    };

    template<class T>
    concept BsdfNode = BsdfBuilder<T> || is_bsdf_mixer_builder<T>::value;


    //**************************************************************************

    template<class First,class Second, ValueBuilder Influence>
    class BsdfMixerBuilder
    {
    public:
        BsdfMixerBuilder(First f, Second s, Influence i) : 
        m_first(f), m_second(s), m_influence(i) {}

        template<BsdfNode T>
        auto SetFirst(T first) { 
            return BsdfMixerBuilder<T,Second,Influence>(first,m_second,m_influence);
        }

        template<BsdfNode T>
        auto SetSecond(T second) { 
            return BsdfMixerBuilder<First,T,Influence>(m_first,second,m_influence);
        }

        auto SetInfluence(ValueBuilder auto influence) { 
            return BsdfMixerBuilder(m_first,m_second,influence);
        }

        auto Build(ShaderCache& cache)
        {
            auto first = detail::Build(m_first,cache);
            auto second = detail::Build(m_second,cache);
            auto influence = detail::Build(m_influence,cache);
            return detail::MixerNode(first,second,influence);
        }

    private:
        First m_first;
        Second m_second;
        Influence m_influence;
    };


}

namespace jpctracer::shader {
    inline auto DefaultMaterialMixer()
    {
        return detail::BsdfMixerBuilder(DefaultDebugShader(),DefaultDebugShader(),0.5);
    }
}