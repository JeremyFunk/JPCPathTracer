#include"geometry.hpp"
#include "handle_vector.hpp"
#include <iostream>
#include"random_geometry.hpp"


template <class T> void print_range(T first, T last)
{
    while (first != last)
    {
        std::cout << *first << ",";
        ++first;
    }
    std::cout << "\n";
}

int main()
{
    auto scn = jpc::random_scene(jpc::random_scene_params{});
    std::cout<<"random scn\n";
    jpc::c::scene_t c_scn = ctype(scn);

    std::cout<<"instance bvh: "<<c_scn.geometries.bvhtree_instances<<"\n";


}

