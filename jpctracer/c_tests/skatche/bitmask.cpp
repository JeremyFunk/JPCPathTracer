#include <cinttypes>
#include <iostream>

int next_idx(std::uint32_t& mask)
{
    int r = __builtin_ffs(mask);
    mask &= mask - 1;
    return r - 1;
}

int main()
{
    std::uint32_t mask = 0x5; // 11
    while (mask != 0)
    {
        int i = next_idx(mask);
        std::cout << i << "\n";
    }
}