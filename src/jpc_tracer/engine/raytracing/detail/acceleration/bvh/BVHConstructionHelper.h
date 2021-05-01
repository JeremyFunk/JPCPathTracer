#pragma once

#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Transformation.h"
#include "jpc_tracer/engine/raytracing/Geometry.h"
#include <algorithm>
#include <numeric>
#include <stdint.h>
#include <vector>

namespace jpctracer::raytracing
{
// Triangle
std::vector<Bounds3D> GenerateTriangleBounds(const TriangleMesh& mesh);
std::vector<Vec3> GetTriangleCenters(const TriangleMesh& mesh);
std::vector<uint32_t> GenerateTriangleMortonCodes(const TriangleMesh& mesh);
void SortTriangleByMortonCode(TriangleMesh& mesh, std::vector<uint32_t>& morton_codes);

// Sphere
std::vector<Bounds3D> GenerateSphereBounds(const SphereMesh& mesh);
std::vector<Vec3> GetSphereCenters(const SphereMesh& mesh);
std::vector<uint32_t> GenerateSphereMortonCodes(const SphereMesh& mesh);
void SortSphereByMortonCode(SphereMesh& mesh, std::vector<uint32_t>& morton_codes);

// Bounding Box
Vec3 GetBoxCenter(const Bounds3D& bound);
uint32_t GetBoxMortonCode(const Bounds3D& bound);

namespace sorthelper
{
std::vector<size_t> GeneratePermutation(const std::vector<uint32_t>& morton_codes);

template <class T> void ApplyPermutationInPlace(const std::vector<size_t>& order, std::vector<T>& vec)
{
    std::vector<bool> done(vec.size());

    for (size_t i = 0; i < vec.size(); i++)
    {
        if (done[i])
            continue;

        done[i] = true;

        // order
        size_t prev_j = i;
        size_t j = order[i];

        while (i != j)
        {
            std::swap(vec[prev_j], vec[j]);
            done[j] = true;

            prev_j = j;
            j = order[j];
        }
    }
}

template <class T, typename... TT>
void ApplyPermutationInPlace(const std::vector<size_t>& order, std::vector<T>& vec, std::vector<TT>&... tt)
{
    ApplyPermutationInPlace(order, vec);
    ApplyPermutationInPlace(order, tt...);
}
} // namespace sorthelper

template <typename... TT> void SortVectorsByMorton(std::vector<uint32_t>& morton_codes, std::vector<TT>&... tt)
{
    const auto order = sorthelper::GeneratePermutation(morton_codes);
    sorthelper::ApplyPermutationInPlace(order, morton_codes);
    sorthelper::ApplyPermutationInPlace(order, tt...);
}
} // namespace jpctracer::raytracing