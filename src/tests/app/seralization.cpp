#include "app/ParseScene.h"
#include "jpc_tracer/engine/PluginsApi.h"
#include "tests/test_utils.h"
#include <gtest/gtest.h>
#include <yaml-cpp/node/parse.h>

namespace jpctracer::parse
{
// clang-format off
const static std::string str_data = "       \n\
Meshes:                                     \n\
  cube:                                     \n\
    path : \"...\"                          \n\
  fancy_tris:                               \n\
    tris : [ [0,0,0] , [0,0,1] ,[0,1,0]]    \n\
Materials:                                  \n\
  mat1:                                     \n\
    type: \"Material1\"                     \n\
    color : [0,1,0]                         \n\
    roughness : 0.1                         \n\
Instances:                                  \n\
  cube:                                     \n\
    rot : [0,0,0]                           \n\
    scl : [1,1,1]                           \n\
    trans : [-1,2,3]                        \n\
    mat0: mat1                              \n\
    mat1: mat1                              \n\
  cube:                                     \n\
    rot : [0,0,0]                           \n\
    scl : 2                                 \n\
    trans : [-1,2,3]                        \n\
    mat0: mat1                              \n\
    mat1: mat1                              \n\
                                            \n\
Lights:                                     \n\
  PointLight:                               \n\
    Pos: []                                 \n\
    Color: []                               \n\
    Intensity: 2                            \n\
                                            \n\
Renderer:                                   \n\
  should_multithread : true                 \n\
  tile_size : 64                            \n\
  static_acceleration : LBVH                \n\
  dynamic_acceleration : NAIVE              \n\
                                            \n\
width : 1920                                \n\
height : 1089                               \n\
result_dir : \"\"                           \n\
";
// clang-format on
TEST(app, deserilizevec3)
{
    YAML::Node node = YAML::Load(str_data);
    Vec3 c;
    Load(node["Instances"]["cube"]["scl"], c);
    TestVec3({1, 1, 1}, c);
}

TEST(app, deserilizetri)
{
    YAML::Node node = YAML::Load(str_data);
    GeometryPtr geometry;
    LoadGeometry(node["Meshes"]["fancy_tris"], geometry);
    auto tri_mesh = std::get<TriangleMesh>(geometry->mesh);
    EXPECT_EQ(tri_mesh.Normals.size(), 1);
    EXPECT_EQ(tri_mesh.TriangleShadings.size(), 1);
    EXPECT_EQ(tri_mesh.TriangleGeometries.size(), 1);

    auto geoms = tri_mesh.TriangleGeometries;
    auto vert1 = tri_mesh.Vertices[geoms[0].Vertex1Id];
    auto vert2 = tri_mesh.Vertices[geoms[0].Vertex2Id];
    auto vert3 = tri_mesh.Vertices[geoms[0].Vertex3Id];

    TestVec3(vert1, {0, 0, 0});
    TestVec3(vert2, {0, 0, 1});
    TestVec3(vert3, {0, 1, 0});
}

TEST(app, deserilizeinstances)
{
}

} // namespace jpctracer::parse