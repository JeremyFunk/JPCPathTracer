#pragma once
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/engine/shadersystem/Shader.h"
#include <memory>
#include <vector>

namespace jpctracer::renderer
{
class ShaderBuffer
{

  public:
    ShaderBuffer() = default;
    ShaderBuffer(const ShaderBuffer&) = delete;
    ShaderBuffer(ShaderBuffer&&) = default;
    uint AddShader(std::shared_ptr<shadersys::IShader> shader);
    const shadersys::IShader* GetShader(uint id) const;

  private:
    std::vector<std::shared_ptr<shadersys::IShader>> m_shaders = {};
};

class MaterialLibrary
{
  public:
    template <shadersys::ShaderFunc T> shadersys::ShaderRef<T> Create();
    const ShaderBuffer& GetBuffer();

  private:
    ShaderBuffer m_shader_buffer;

    shadersys::TextureBuffer m_texture_buffer;
};

template <shadersys::ShaderFunc T> shadersys::ShaderRef<T> MaterialLibrary::Create()
{
    auto shader = std::make_shared<shadersys::Shader<T>>();
    uint id = m_shader_buffer.AddShader(shader);
    return shadersys::ShaderRef(shader, m_texture_buffer, id);
}

} // namespace jpctracer::renderer