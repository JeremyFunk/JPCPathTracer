#include <iostream>
#include <omp.h>

int main()
{
    int a[4] = {1,2,3,4};
    int b[4] ={3,4,5,6};
    int c[4] ={0}; 
    #pragma omp simd
    for(int i=0;i<4;i++)
        c[i]=a[i]+b[i];

    std::cout<<c[0]<<c[1]<<c[2]<<c[3]<<std::endl;

    return 0;
}