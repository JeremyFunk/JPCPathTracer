#pragma once

#include "jpc_tracer/core/core.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/plugins/shaders/cache/ShaderCache.h"
#include "../Concepts.h"
#include <cassert>

namespace jpctracer::shader {
    
    namespace detail {

    template<class First,class Second, Value Influence>
    struct Mixer
    {
        Mixer(First f,Second s,Influence i): first(f),second(s),influence(i){}
        First first;
        Second second;
        Influence influence;

        auto Eval(const SurfaceInteraction& interaction)
        {
            Prec a = detail::Eval(influence,interaction);
            assert(a>=0);
            assert(a<1);
            auto f = detail::Eval(first,interaction);
            auto s = detail::Eval(second,interaction);

            return f*a+s*(1-a);
        }
    };

    
    
    template<class First,class Second, ValueBuilder Influence>
        requires SameBuilder<First,Second>
    class MixerBuilder
    {
    public:
        MixerBuilder(First first, Second second, Influence influence)
            : m_first(first), m_second(second), m_influence(influence) {}
            
        auto SetFirst(SameBuilder<Second> auto first) { 
            return MixerBuilder(first,m_second,m_influence);
        }

        auto SetSecond(SameBuilder<First> auto second) { 
            return MixerBuilder(m_first,second,m_influence);
        }

        auto SetInfluence(ValueBuilder auto influence) { 
            return MixerBuilder(m_first,m_second,influence);
        }

        auto Build(ShaderCache& cache)
        {
            auto first = detail::Build(m_first,cache);
            auto second = detail::Build(m_second,cache);
            auto influence = detail::Build(m_influence,cache);
            return detail::Mixer(first,second,influence);
        }

    private:
        First m_first;
        Second m_second;
        Influence m_influence;
    };

    }

    inline ColorBuilder auto DefaultColorMixer()
    {
        return detail::MixerBuilder{Black(),Black(),0.5};
    };

    inline ValueBuilder auto DefaultValueMixer()
    {
        return detail::MixerBuilder{1.0,0.2,0.5};
    };

    inline NormalBuilder auto DefaultValueNormal()
    {
        return detail::MixerBuilder{Vec3(),Vec3(),0.5};
    };
    


}