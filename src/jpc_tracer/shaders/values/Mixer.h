#pragma once

#include "jpc_tracer/maths/maths.h"
#include "jpc_tracer/shaders/Cache.h"
#include "Concepts.h"

namespace jpctracer {

    namespace shader {

    template<class First,class Second, Value Influence>
        requires shader::SameBuilder<First, Second>
    struct Mixer
    {
        Mixer(First f,Second s,Influence i): first(f),second(s),influence(i){}
        First first;
        Second second;
        Influence influence;

        auto Eval(const SurfaceInteraction& interaction)
        {
            Prec a = jpctracer::shader::Eval(influence,interaction);
            auto f = jpctracer::shader::Eval(first,interaction);
            auto s = jpctracer::shader::Eval(second,interaction);

            return f*a+s*(1-a);
        }
    };

    }

    template<class First,class Second, ValueBuilder Influence>
        requires shader::SameBuilder<First,Second>
    class MixerBuilder
    {
    public:
        MixerBuilder(First first, Second second, Influence influence)
            : m_first(first), m_second(second), m_influence(influence) {}
            
        auto SetFrist(shader::SameBuilder<Second> auto first) { 
            return MixerBuilder(first,m_second,m_influence);
        }

        auto SetSecond(shader::SameBuilder<First> auto second) { 
            return MixerBuilder(m_first,second,m_influence);
        }

        auto SetInfluence(ValueBuilder auto influence) { 
            return MixerBuilder(m_first,m_second,influence);
        }

        auto Build(shader::Cache& cache)
        {
            auto first = shader::Build(m_first,cache);
            auto second = shader::Build(m_second,cache);
            auto influence = shader::Build(m_influence,cache);
            return shader::Mixer(first,second,influence);
        }

    private:
        First m_first;
        Second m_second;
        Influence m_influence;
    };

    ColorBuilder auto DefaultColorMixer()
    {
        return MixerBuilder{Spectrum{},Spectrum{},0.5};
    };

    ValueBuilder auto DefaultValueMixer()
    {
        return MixerBuilder{1.0,0.2,0.5};
    };

    NormalBuilder auto DefaultValueNormal()
    {
        return MixerBuilder{Vec3(),Vec3(),0.5};
    };
    


}