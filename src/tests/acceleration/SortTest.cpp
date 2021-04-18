#include "gtest/gtest.h"
#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>
#include <stdint.h>

#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/engine/raytracing/detail/acceleration/bvh/BVHConstructionHelper.h"

namespace jpctracer {

    template <class T>
    void ApplyPermutationInPlace(std::vector<T>& vec, const std::vector<size_t>& order)
    {
        std::vector<bool> done(vec.size());

        for (size_t i = 0; i < vec.size(); i++)
        {
            if (done[i]) continue;

            done[i] = true;

            // order geometry
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

    TEST(LBVH, SortTest)
    {
        std::vector<uint32_t> morton = {10, 2, 3, 7, 6, 1, 4, 8, 5, 9};
        
        auto order = raytracing::sorthelper::GeneratePermutation(morton);

        std::vector<float> vec = {10, 2, 3, 7, 6, 1, 4, 8, 5, 9};

        // raytracing::sorthelper::ApplyPermutationInPlace<Prec>(vec, order);

        ApplyPermutationInPlace<float>(vec, order);

        std::vector<float> comp = {1,2,3,4,5,6,7,8,9,10};

        EXPECT_EQ(vec, comp);
    }
}