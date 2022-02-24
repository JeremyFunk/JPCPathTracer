#include <jpc_api.h>

static inline int next_mask_idx(uint mask)
{
    int r = __builtin_ffs(mask);
    mask &= mask - 1;
    return r - 1;
}