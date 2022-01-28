#pragma once
#include "cglm/cglm.h"
#include "jpc_api.h"
#include <math.h>
#include <stdbool.h>
inline bool solveQuadratic(const float a,
                           const float b,
                           const float c,
                           float*      x0,
                           float*      x1)
{
    float discr = b * b - 4 * a * c;
    if (discr < 0)
        return false;
    else if (discr == 0)
        *x0 = *x1 = -0.5 * b / a;
    else
    {
        float q = (b > 0) ? -0.5 * (b + sqrt(discr)) : -0.5 * (b - sqrt(discr));
        *x0 = q / a;
        *x1 = c / q;
    }
    if (*x0 > *x1)
    {
        float temp = *x0;
        *x0 = *x1;
        *x1 = temp;
    }

    return true;
}
inline void interpolate3d(vec3 v1, vec3 v2, vec3 v3, float u, float v, vec3 dst)
{

    // u * v2 + v * v3 + (1 - u - v) * v1;
    vec3 temp;
    glm_vec3_scale(v2, u, dst);
    glm_vec3_scale(v3, v, temp);
    glm_vec3_add(temp, dst, dst);
    glm_vec3_scale(v1, (1 - u - v), temp);
    glm_vec3_add(temp, dst, dst);
}

inline void interpolate2d(vec2 v1, vec2 v2, vec2 v3, float u, float v, vec2 dst)
{

    // u * v2 + v * v2 + (1 - u - v) * v1;
    vec2 temp;
    glm_vec2_scale(v2, u, dst);
    glm_vec2_scale(v3, v, temp);
    glm_vec2_add(temp, dst, dst);
    glm_vec2_scale(v1, (1 - u - v), temp);
    glm_vec2_add(temp, dst, dst);
}
inline void mat4_ucopy(float4x4 mat, mat4 dest)
{
    dest[0][0] = mat[0][0];
    dest[1][0] = mat[1][0];
    dest[0][1] = mat[0][1];
    dest[1][1] = mat[1][1];
    dest[0][2] = mat[0][2];
    dest[1][2] = mat[1][2];
    dest[0][3] = mat[0][3];
    dest[1][3] = mat[1][3];

    dest[2][0] = mat[2][0];
    dest[3][0] = mat[3][0];
    dest[2][1] = mat[2][1];
    dest[3][1] = mat[3][1];
    dest[2][2] = mat[2][2];
    dest[3][2] = mat[3][2];
    dest[2][3] = mat[2][3];
    dest[3][3] = mat[3][3];
}