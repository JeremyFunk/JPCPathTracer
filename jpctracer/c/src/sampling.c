#include "sampling.h"

void concentric_sample_disk(vec2 random_point, vec2 out)
{
    float rand_1 = random_point[0] * 2 - 1;
    float rand_2 = random_point[1] * 2 - 1;

    if (rand_1 == 0 && rand_2 == 0)
    {
        out[0] = 0;
        out[1] = 0;
    }
    else
    {
        float radius, theta;
        if (fabs(rand_1) > fabs(rand_2))
        {
            radius = rand_1;
            theta = M_PI / 4 * rand_2 / rand_1;
        }
        else
        {
            radius = rand_2;
            theta = M_PI / 2 - M_PI / 4 * rand_1 / rand_2;
        }
        out[0] = cos(theta);
        out[0] = sin(theta);
        glm_vec2_scale(out, radius, out);
    }
}

void cosinus_sample_hemisphere(vec2 random_point, vec3 out)
{
    vec2 d;
    concentric_sample_disk(random_point, d);
    float z = sqrt(fmax(0.f, 1 - d[0] * d[0] - d[1] * d[1]));
    out[0] = d[0];
    out[1] = d[1];
    out[2] = z;
}

// updates rand_p, to be reused
int sample_discrete(float* pdfs, int n, float* rand_p)
{
    float sum = 0;
    int   i = 0;
    for (i = 0; i < (n - 1) && *rand_p > sum + pdfs[i]; i++)
    {
        sum += pdfs[i];
    }
    *rand_p = (*rand_p - sum) / pdfs[i];
    if (*rand_p > 1. + 1e-6 || *rand_p < -1e-6)
    {
        printf("rand_p: %f sum: %f pdf: %f\n", *rand_p, sum, pdfs[i]);
    }
    assert(*rand_p <= 1. + 1e-6);

    assert(*rand_p >= -1e-6);
    return i;
}
