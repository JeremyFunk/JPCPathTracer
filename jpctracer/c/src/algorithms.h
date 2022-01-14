#pragma once
#include "cglm/types.h"
#include "cglm/vec3.h"
#include <cglm/cglm.h>

// inplace
// mat*{vecs[i],last} = {vecs[i],0}
inline void mat_mul_n(mat4 mat, vec3* vecs, uint n, float last)
{
    vec3 temp;
    for (int i = 0; i < n; i++)
    {
        glm_mat4_mulv3(mat, vecs[i], last, temp);
        glm_vec3_copy(temp, vecs[i]);
    }
}