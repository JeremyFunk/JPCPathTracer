#include "texture.h"
#include "cglm/util.h"
#include "cglm/vec3-ext.h"

#include <assert.h>
#include <cglm/cglm.h>

float* texture_get_idx(const image_t tex, int x, int y)
{
    return &tex.data[y * tex.width * tex.channels + x * tex.height * tex.channels];
}

typedef struct
{
    float* pixels[2][2];
    float  t_x;
    float  t_y;
} pixels_t;

pixels_t set_pixels(const image_t tex, float2 uv)
{

    pixels_t p;

    float x = uv[0] * tex.width;
    float y = uv[1] * tex.height;

    int x_low = x;
    int x_up = ceil(x);
    int y_low = y;
    int y_up = ceil(y);

    p.t_x = x - (float)x_low;
    p.t_y = y - (float)y_low;

    p.pixels[0][0] = texture_get_idx(tex, x_low, y_low);
    p.pixels[0][1] = texture_get_idx(tex, x_up, y_low);
    p.pixels[1][0] = texture_get_idx(tex, x_low, y_up);
    p.pixels[1][1] = texture_get_idx(tex, x_up, y_up);
    return p;
}

void texture_get_float4(const image_t tex, float2 uv, float4 dst)
{

    assert(tex.channels >= 4);
    pixels_t p = set_pixels(tex, uv);

    float4 temp1, temp2;

    glm_vec4_lerp(p.pixels[0][0], p.pixels[0][1], p.t_x, temp1);

    glm_vec4_lerp(p.pixels[1][0], p.pixels[1][1], p.t_x, temp2);

    glm_vec4_lerp(temp1, temp2, p.t_y, dst);
}

void texture_get_float3(const image_t tex, float2 uv, float3 dst)
{
    assert(tex.channels >= 3);
    pixels_t p = set_pixels(tex, uv);

    float3 temp1, temp2;

    glm_vec3_lerp(p.pixels[0][0], p.pixels[0][1], p.t_x, temp1);

    glm_vec3_lerp(p.pixels[1][0], p.pixels[1][1], p.t_x, temp2);

    glm_vec3_lerp(temp1, temp2, p.t_y, dst);
}

void texture_get_float(const image_t tex, float2 uv, float* dst)
{

    pixels_t p = set_pixels(tex, uv);

    float p_means[2][2];

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            switch (tex.channels)
            {
            case 1:
                p_means[i][j] = p.pixels[i][j][0];
                break;
            case 3:
                p_means[i][j] = glm_vec3_hadd(p.pixels[i][j]) / 3.;
                break;
            case 4:
                p_means[i][j] = glm_vec4_hadd(p.pixels[i][j]) / 4.;
            default:
                assert(false);
            }
        }
    }

    float temp1,temp2;
    temp1 = glm_lerp(p_means[0][0],p_means[0][1],p.t_x);
    temp2 = glm_lerp(p_means[1][0],p_means[1][1],p.t_x);

    *dst = glm_lerp(temp1,temp2,p.t_y);
}
