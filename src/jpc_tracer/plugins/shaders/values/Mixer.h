#pragma once

#include "jpc_tracer/core/core.h"
#include "jpc_tracer/plugins/shaders/cache/ShaderCache.h"
#include "Concepts.h"
#include <cassert>

namespace jpctracer {
    
    namespace plugins {

    template<class First,class Second, Value Influence>
    struct Mixer
    {
        Mixer(First f,Second s,Influence i): first(f),second(s),influence(i){}
        First first;
        Second second;
        Influence influence;

        auto Eval(const SurfaceInteraction& interaction)
        {
            Prec a = jpctracer::plugins::Eval(influence,interaction);
            assert(a>=0);
            assert(a<1);
            auto f = jpctracer::plugins::Eval(first,interaction);
            auto s = jpctracer::plugins::Eval(second,interaction);

            return f*a+s*(1-a);
        }
    };

    }
    
    template<class First,class Second, ValueBuilder Influence>
        requires plugins::SameBuilder<First,Second>
    class MixerBuilder
    {
    public:
        MixerBuilder(First first, Second second, Influence influence)
            : m_first(first), m_second(second), m_influence(influence) {}
            
        auto SetFrist(plugins::SameBuilder<Second> auto first) { 
            return MixerBuilder(first,m_second,m_influence);
        }

        auto SetSecond(plugins::SameBuilder<First> auto second) { 
            return MixerBuilder(m_first,second,m_influence);
        }

        auto SetInfluence(ValueBuilder auto influence) { 
            return MixerBuilder(m_first,m_second,influence);
        }

        auto Build(plugins::ShaderCache& cache)
        {
            auto first = plugins::Build(m_first,cache);
            auto second = plugins::Build(m_second,cache);
            auto influence = plugins::Build(m_influence,cache);
            return plugins::Mixer(first,second,influence);
        }

    private:
        First m_first;
        Second m_second;
        Influence m_influence;
    };

    inline ColorBuilder auto DefaultColorMixer()
    {
        return MixerBuilder{Spectrum{},Spectrum{},0.5};
    };

    inline ValueBuilder auto DefaultValueMixer()
    {
        return MixerBuilder{1.0,0.2,0.5};
    };

    inline NormalBuilder auto DefaultValueNormal()
    {
        return MixerBuilder{Vec3(),Vec3(),0.5};
    };
    


}