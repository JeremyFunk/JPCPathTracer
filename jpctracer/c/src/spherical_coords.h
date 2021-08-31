#pragma once
#include "cglm/struct/vec3.h"
#include "cglm/util.h"
#include "cglm/vec3.h"
#include "jpc_api.h"
#include <cglm/cglm.h>
#include <cglm/struct.h>
#include <math.h>

static inline vec3s test()
{
    return (vec3s){1, 2, 3};
}

// Returns normalized Vector
static inline vec3s spherical_direction(float sin_theta, float cos_theta, float phi)
{

    return (vec3s){sin_theta * cos(phi), sin_theta * sin(phi), cos_theta};
}

static inline float spherical_theta(const vec3 normal)
{
    return glm_clamp(normal[2], -1, 1);
}
static inline float spherical_phi(const vec3 normal)
{
    float p = atan2(normal[1], normal[0]);
    return (p < 0) ? (p + 2 * M_PI) : M_PI;
}

static inline float cos_theta(const vec3 normal)
{
    return normal[2];
}

static inline float cos2theta(const vec3 normal)
{
    return normal[2] * normal[2];
}

static inline float abs_cos_theta(const vec3 normal)
{
    return fabs(normal[2]);
}

static inline float sin2theta(const vec3 normal)
{
    return fmax((float)0, (float)1 - cos2theta(normal));
}

static inline float sin_theta(const vec3 normal)
{
    return sqrt(sin2theta(normal));
}

static inline float tan_theta(const vec3 normal)
{
    return sin_theta(normal) / cos_theta(normal);
}

static inline float tan2theta_n(const vec3 normal)
{
    return sin2theta(normal) / cos2theta(normal);
}

static inline float tan2theta(float cos2_theta)
{
    float sin2_theta = 1 - cos2_theta;
    float tan2_theta = sin2_theta / cos2_theta;
    return tan2_theta;
}

static inline float cos_phi(const vec3 normal)
{
    float sinTheta = sin_theta(normal);
    return (sinTheta == 0) ? 1 : glm_clamp(normal[0] / sinTheta, -1, 1);
}

static inline float sin_phi(const vec3 normal)
{
    float sinTheta = sin_theta(normal);
    return (sinTheta == 0) ? 0 : glm_clamp(normal[1] / sinTheta, -1, 1);
}

static inline float cos2phi(const vec3 normal)
{
    return cos_phi(normal) * cos_phi(normal);
}

static inline float sin2phi(const vec3 normal)
{
    return sin_phi(normal) * sin_phi(normal);
}

static inline bool same_hemisphere_nspace(const vec3 v1, const vec3 v2)
{
    return v1[2] * v2[2] >= 0;
}

static inline vec3s reflect(vec3 v, vec3 normal)
{
    // -v+2* v dot normal
    vec3s result;
    glm_vec3_copy(v, result.raw);
    glm_vec3_negate(result.raw);
    float v_dot_normal = 2 * glm_vec3_dot(v, normal);
    result = glms_vec3_adds(result, v_dot_normal);
    return result;
}
