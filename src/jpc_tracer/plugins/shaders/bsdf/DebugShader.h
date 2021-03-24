#pragma once
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/plugins/shaders/BaseFunctions.h"
#include "jpc_tracer/plugins/shaders/Concepts.h"
#include "jpc_tracer/plugins/shaders/cache/ShaderCache.h"

namespace jpctracer::shader::detail
{
    struct DebugShaderClosure : public IBsdfClosure
    {
        DebugShaderClosure(Spectrum color);
        DebugShaderClosure();
        virtual std::pair<Spectrum,Prec> operator()(const SharedMemory& mem,
                            const Ray& incident_ray) const;

        virtual std::pair<Spectrum,Prec> operator()(const SharedMemory& mem,
                        Ray* out_incident_ray, Vec2 random_point) const;
        
        Spectrum m_color;
    };

    static_assert(BsdfClosure<DebugShaderClosure>);

    template<Color ColorT>
    struct DebugShader
    {
        DebugShader(ColorT c) : m_color(c) {}

        template<MaterialType type>
            requires(type==MaterialType::BSDF || type ==MaterialType::DIFFUSE)
        auto CreateDistribution(const SharedMemory& memory) const
        {
            Spectrum color = Eval(m_color,memory.Interaction);
            return DebugShaderClosure(color);
        }
        ColorT m_color;
        
    };

    static_assert(BsdfCreator<DebugShader<Spectrum>>, "DebugShader is no BsdfCreator");


    template<ColorBuilder ColorT>
    struct DebugShaderBuilder
    {
        DebugShaderBuilder(ColorT c) : m_color_builder(c) {}

        auto SetColor(ColorBuilder auto builder)
        {
            return DebugShaderBuilder(builder);
        }

        auto Build(ShaderCache& cache)
        {
            return DebugShader(detail::Build(m_color_builder,cache));
        }

        ColorT m_color_builder;
    };
    static_assert(BsdfBuilder<DebugShaderBuilder<Spectrum>>) ;   
}

namespace jpctracer::shader {
    inline auto DefaultDebugShader()
    {
        return detail::DebugShaderBuilder{Black()};
    }
}