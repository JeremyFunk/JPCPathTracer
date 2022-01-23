#pragma once
#include "cglm/cglm.h"
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
void interpolate3d(vec3 v1, vec3 v2, vec3 v3, float u, float v, vec3 dst)
{

    // u * v2 + v * v3 + (1 - u - v) * v1;
    vec3 temp;
    glm_vec3_scale(v2, u, dst);
    glm_vec3_scale(v3, v, temp);
    glm_vec3_add(temp, dst, dst);
    glm_vec3_scale(v1, (1 - u - v), temp);
    glm_vec3_add(temp, dst, dst);
}

void interpolate2d(vec2 v1, vec2 v2, vec2 v3, float u, float v, vec2 dst)
{

    // u * v2 + v * v2 + (1 - u - v) * v1;
    vec2 temp;
    glm_vec2_scale(v2, u, dst);
    glm_vec2_scale(v3, v, temp);
    glm_vec2_add(temp, dst, dst);
    glm_vec2_scale(v1, (1 - u - v), temp);
    glm_vec2_add(temp, dst, dst);
}