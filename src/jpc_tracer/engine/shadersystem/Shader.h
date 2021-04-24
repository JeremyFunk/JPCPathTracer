#pragma once

#include "BsdfClosure.h"
#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/engine/shadersystem/TextureBuffer.h"
#include <concepts>
#include <cstdint>
#include <memory>
#include <string>

namespace jpctracer::shadersys
{

template <class T> concept ShaderFunc = requires(T f, Ray scattered)
{
    T(); // default constructable
    // clang-format off
    {f.bsdf()}->Closure;
    // clang-format on
};

struct IShader
{
    virtual void Eval(const SurfaceInteraction& interaction, Ray scattered_ray, View<Ray> rays,
                      SeperatedBsdfs& result) const = 0;
    virtual void Eval(const SurfaceInteraction& interaction, Ray scattered_ray, View<Ray> rays,
                      CombinedBsdfs& result) const = 0;
    virtual void Sample(const SurfaceInteraction& interaction, Ray scattered_ray, View<Ray> rays, View<Vec2> samples,
                        SeperatedBsdfs& result) const = 0;
    virtual void Sample(const SurfaceInteraction& interaction, Ray scattered_ray, View<Ray> rays, View<Vec2> samples,
                        CombinedBsdfs& result) const = 0;
    virtual Spectrum Emission(const SurfaceInteraction& interaction) const = 0;
    virtual Prec Transparency(const SurfaceInteraction& interaction) const = 0;
    virtual ~IShader()
    {
    }
};

template <ShaderFunc T> struct Shader final : public IShader
{
    T default_shader;
    std::vector<std::pair<Texture, std::uintptr_t>> color_bindings;
    std::vector<std::pair<Texture, std::uintptr_t>> value_bindings;
    std::vector<std::pair<Texture, std::uintptr_t>> normal_bindings;

    void init_shader_func(T* shader, const SurfaceInteraction& interaction) const
    {
        std::uintptr_t start = (std::uintptr_t)shader;
        for (const auto& binding : color_bindings)
            *((Spectrum*)(start + binding.second)) = binding.first(interaction.UV);
        /*for (const auto& binding : value_bindings)
            *((Prec*)shader + binding.second) = binding.first(interaction.UV);
        for (const auto& binding : normal_bindings)
            *((Norm3*)shader + binding.second) = binding.first(interaction.UV);*/
    }

    void Eval(const SurfaceInteraction& interaction, Ray scattered_ray, View<Ray> rays, SeperatedBsdfs& result) const
    {
        T shader = default_shader;
        init_shader_func(&shader, interaction);
        EvalShader(shader.bsdf(), scattered_ray, rays, result);
    }
    void Eval(const SurfaceInteraction& interaction, Ray scattered_ray, View<Ray> rays, CombinedBsdfs& result) const
    {
        T shader = default_shader;
        init_shader_func(&shader, interaction);
        EvalShader(shader.bsdf(), scattered_ray, rays, result);
    }

    void Sample(const SurfaceInteraction& interaction, Ray scattered_ray, View<Ray> rays, View<Vec2> samples,
                SeperatedBsdfs& result) const
    {
        T shader = default_shader;
        init_shader_func(&shader, interaction);
        SampleShader(shader.bsdf(), scattered_ray, rays, samples, result);
    }
    void Sample(const SurfaceInteraction& interaction, Ray scattered_ray, View<Ray> rays, View<Vec2> samples,
                CombinedBsdfs& result) const
    {
        T shader = default_shader;
        init_shader_func(&shader, interaction);
        SampleShader(shader.bsdf(), scattered_ray, rays, samples, result);
    }

    Spectrum Emission(const SurfaceInteraction& interaction) const
    {
        T shader = default_shader;
        init_shader_func(&shader, interaction);
        return EmissionShader(shader.bsdf());
    }
    Prec Transparency(const SurfaceInteraction& interaction) const
    {
        T shader = default_shader;
        init_shader_func(&shader, interaction);
        return TransparencyShader(shader.bsdf());
    }
};

struct IShaderRef
{
    uint Id;
};

template <ShaderFunc T> class ShaderRef
{
  public:
    ShaderRef(std::shared_ptr<Shader<T>> shader, TextureBuffer& tex_buffer, uint id)
        : m_shader(shader), Id(id), m_tex_buffer(tex_buffer)
    {
    }

    operator IShaderRef()
    {
        return IShaderRef{Id};
    }
    T* operator->()
    {
        return &m_shader->default_shader;
    }

    // returns false if Spectrum* is no property of *this
    bool BindTexture(Spectrum* property, std::string texture_path)
    {
        std::uintptr_t redirect_prop;
        if (to_property_pointer(property, redirect_prop))
        {
            Texture texture = m_tex_buffer.Load(texture_path);
            m_shader->color_bindings.push_back({texture, redirect_prop});
            return true;
        }
        JPC_LOG_ERROR("Could not bind texture");
        return false;
    }
    // returns false if Spectrum* is no property of *this
    bool BindTexture(Prec* property, std::string texture_path)
    {
        std::uintptr_t redirect_prop;
        if (to_property_pointer(property, redirect_prop))
        {
            Texture texture = m_tex_buffer.Load(texture_path);
            m_shader->value_bindings.push_back({texture, redirect_prop});
            return true;
        }
        JPC_LOG_ERROR("Could not bind texture");
        return false;
    }
    // returns false if Spectrum* is no property of *this
    bool BindTexture(Norm3* property, std::string texture_path)
    {
        std::uintptr_t redirect_prop;
        if (to_property_pointer(property, redirect_prop))
        {
            Texture texture = m_tex_buffer.Load(texture_path);
            m_shader->normal_bindings.push_back({texture, redirect_prop});
            return true;
        }
        JPC_LOG_ERROR("Could not bind texture");
        return false;
    }

    const uint Id;

  private:
    template <class propT> bool to_property_pointer(propT* property, std::uintptr_t& result)
    {
        // check if points to shader
        std::uintptr_t dest_shader = (std::uintptr_t)&m_shader->default_shader;
        std::uintptr_t prop = (std::uintptr_t)property;
        if (dest_shader <= prop && prop + sizeof(propT) <= dest_shader + sizeof(T))
        {
            result = prop - dest_shader;
            return true;
        }
        return false;
    }
    std::shared_ptr<Shader<T>> m_shader;
    TextureBuffer& m_tex_buffer;
};

} // namespace jpctracer::shadersys