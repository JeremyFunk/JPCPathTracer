#pragma once
#include "jpc_tracer/engine/films/Film.h"
#include "jpc_tracer/engine/renderer/RenderInterfaces.h"
#include "jpc_tracer/engine/renderer/Tracer.h"
#include "jpc_tracer/engine/shadersystem/ShaderResults.h"
#include "jpc_tracer/engine/utilities/SamplingRoutins.h"
#include "jpc_tracer/engine/utilities/SphericalCoordinates.h"
#include "shadersystem/shadersystem.h"

#include "raytracing/Geometry.h"
#include <utility>

namespace jpctracer
{

using IBsdfClosure = shadersys::IBsdfClosure;
using BsdfNode = shadersys::BsdfNode;
using LightResults = shadersys::LightResults;
template <class T> using Distributed = shadersys::Distributed<T>;

template <class T> using ShaderResults = shadersys::ShaderResults<T>;
template <class T> concept JPCShader = shadersys::ShaderFunc<T>;

template <std::derived_from<IBsdfClosure> T> inline BsdfNode CreateBsdf(MaterialType type, const T& bsdf)
{
    return shadersys::__CreateBsdf(type, bsdf);
}

inline BsdfNode Mix(BsdfNode node1, BsdfNode node2, Prec weight)
{
    return shadersys::MixBsdf(node1, node2, weight);
}

using IRayBehavior = renderer::IRayBehavior;
using Tracer = renderer::Tracer;
using HitPoint = shadersys::HitPoint;

using TriangleGeometry = raytracing::TriangleGeometry;
using TriangleShading = raytracing::TriangleShading;
using TriangleMesh = raytracing::TriangleMesh;
using SphereMesh = raytracing::SphereMesh;

using Film = film::Film;

using ICamera = renderer::ICamera;
using ISampler = renderer::ISampler;
using IIntegrator = renderer::IIntegrator;

constexpr bool IsDeltaDistribution(Prec pdf)
{
    return pdf < 0.0001;
}

} // namespace jpctracer