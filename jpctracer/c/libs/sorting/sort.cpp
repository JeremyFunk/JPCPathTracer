#include "sort.h"
#include <algorithm>
#include <numeric>
#include <stdio.h>
#include <string.h>
#include <vector>

void sort_permutation_uint(uint32_t* data, uint32_t* permutation, uint n)
{
    std::iota(permutation, permutation + n, 0);
    std::sort(permutation, permutation + n,
              [&](uint32_t a, uint32_t b) { return data[a] < data[b]; });
    apply_permutation(permutation,data,n,sizeof(uint32_t));

}


void apply_permutation(uint32_t* permutation, void* dst, uint n, size_t size)
{

    std::vector<bool> done(n,false);
    void* temp = malloc(size);
    char* data = (char*) dst;


    for (size_t i = 0; i < n; i++)
    {
        if (done[i])
            continue;

        done[i] = true;

        // order
        size_t prev_j = i;
        size_t j = permutation[i];

        while (i != j)
        {
            void* curr = data+j*size;
            void* prev = data+prev_j*size;
            //swap
            memcpy(temp, curr,size);
            memcpy(curr,prev,size);
            memcpy(prev,temp,size);

            done[j] = true;

            prev_j = j;
            j = permutation[j];
        }
    }
    free(temp);
}
