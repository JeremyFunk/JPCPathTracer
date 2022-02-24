extern "C"
{
#include "geom_log.h"
#include "cglm/cglm.h"
}
#include <array>
#include <atomic>
#include <cstdio>
#include <mutex>
#include <string>
#include <vector>

struct cube
{
    std::array<vec3, 8> verticies;
    // layout:
    /*

    View: x y

    y
    ^   v3/4-------v5/6
    |   |           |
    |   |           |
    |   v1/2-------v7/8
    --------------> x


    z
    ^   v2/4-------v8/6
    |   |           |
    |   |           |
    |   v1/3-------v7/5
    --------------> x


*/
    std::string name;
    cube() = default;
    cube(vec3* verts, std::string _name)
    {
        for (int i = 0; i < 8; i++)
        {

            glm_vec3_copy(verts[i], verticies[i]);
        }
        name = _name;
    }
    cube(const cube& c) = default;
    cube(cube&& c) = default;
    cube& operator=(const cube& c) = default;
    ~cube() = default;
};

std::vector<cube> s_cubes = {};
std::mutex        s_cubes_lock;

void p_log_cube(const char* name, vec3 verticies[8])
{

    s_cubes_lock.lock();
    s_cubes.push_back(cube{verticies, std::string(name)});
    s_cubes_lock.unlock();
}

void p_log_bounds(const char* name, const float* min, const float* max)
{
    vec3 data[8] = {
        {min[0], min[1], min[2]}, // v1
        {min[0], min[1], max[2]}, // v2
        {min[0], max[1], min[2]}, // v3
        {min[0], max[1], max[2]}, // v4
        {max[0], max[1], min[2]}, // v5
        {max[0], max[1], max[2]}, // v6
        {max[0], min[1], min[2]}, // v7
        {max[0], min[1], max[2]}  // v8
    };
    p_log_cube(name, data);
}
// returns the offset
int write_cube(FILE* file, const cube& c, int face_offset)
{

    fprintf(file, "o %s\n", c.name.data());
    for (int i = 0; i < 8; i++)
    {
        fprintf(file,
                "v %f %f %f\n",
                c.verticies[i][0],
                c.verticies[i][1],
                c.verticies[i][2]);
    }
    int o = face_offset;
    fprintf(file, "s off\n"); // smoothing of
    fprintf(file, "f %d %d %d %d \n", 1 + o, 3 + o, 5 + o, 7 + o);
    fprintf(file, "f %d %d %d %d \n", 2 + o, 4 + o, 6 + o, 8 + o);
    fprintf(file, "f %d %d %d %d \n", 1 + o, 3 + o, 4 + o, 2 + o);

    fprintf(file, "f %d %d %d %d \n", 3 + o, 4 + o, 6 + o, 5 + o);
    fprintf(file, "f %d %d %d %d \n", 5 + o, 7 + o, 8 + o, 6 + o);
    fprintf(file, "f %d %d %d %d \n", 1 + o, 2 + o, 8 + o, 7 + o);
    return face_offset + 8;
};
void geom_log_write(const char* filepath)
{
    FILE* file = fopen(filepath, "w");
    s_cubes_lock.lock();
    int offset = 0;
    for (const cube& c : s_cubes)
    {
        offset = write_cube(file, c, offset);
    }
    s_cubes.clear();
    s_cubes_lock.unlock();
    fclose(file);
}
