#pragma once
#include "renderer/JPCRenderer.h"
#include "utilities/MeshIO.h"
namespace jpctracer
{
using JPCRenderer = renderer::JPCRenderer;
using Geometry = renderer::Geometry;
using IShader = shadersys::IShader;
template <class T> using ShaderRef = shadersys::ShaderRef<T>;
using IShaderRef = shadersys::IShaderRef;
} // namespace jpctracer