#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include <stdlib.h>

void sort_permutation_uint(uint32_t* data, uint32_t* permutation,uint n);
void apply_permutation(uint32_t* permutation, void* dst, uint n, size_t size);

#ifdef __cplusplus
}
#endif
