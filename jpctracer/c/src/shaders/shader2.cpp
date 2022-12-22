#include "jpc_api.h"
#include <vector>
extern "C"
{
#include "../bsdf.h"
#include "../shaders.h"
#include "../utils.h"
}

struct params
{
    float test[3] = {0};
    float lol[2] = {0, 1};

    bsdfnode_t eval(bsdfcontext_t* ctx)
    {
        return diffuse(ctx, test);
    }
};

struct registery
{
    std::vector<uniform_desc_t> layout;
    std::size_t                 size;

    template <class T> void add()
};

void register_params()
{
    registery r;
}