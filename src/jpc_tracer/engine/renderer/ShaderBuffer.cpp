#include "ShaderBuffer.h"
#include <memory>
#include <optional>

namespace jpctracer::renderer
{
uint ShaderBuffer::AddShader(std::shared_ptr<shadersys::IShader> shader)
{
    m_shaders.emplace_back(shader);
    return m_shaders.size() - 1;
}

const shadersys::IShader* ShaderBuffer::GetShader(uint id) const
{
    return m_shaders[id].get();
}

const ShaderBuffer& MaterialLibrary::GetBuffer()
{
    return m_shader_buffer;
}

} // namespace jpctracer::renderer