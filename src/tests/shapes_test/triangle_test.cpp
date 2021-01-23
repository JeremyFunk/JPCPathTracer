#include "gtest/gtest.h"
#include <memory>
#include <vector>
#include "core/Bounds3D.h"
#include "core/Linalg.h"
#include "shapes/TriangleMesh.h"

namespace jpc_tracer
{
    auto get_mesh()
    {
        std::vector<Prec> vertices = {0,0,0, 1,1,1, 2,2,2};
        std::vector<Prec> normals = {0,0,0, 1,1,1, 2,2,2};
        std::vector<Prec> uvs= {0,0,0, 1,1,1, 2,2,2};
        std::vector<int> indices= {0,0,0, 3,3,3, 6,6,6};

        auto ptr_vertices = std::make_shared<std::vector<Prec>>(vertices);
        auto ptr_normals = std::make_shared<std::vector<Prec>>(normals);
        auto ptr_uvs = std::make_shared<std::vector<Prec>>(uvs);
        auto ptr_indices = std::make_shared<std::vector<int>>(indices);

        std::shared_ptr<TriangleMesh> mesh = std::make_shared<TriangleMesh>(std::make_shared<std::vector<Prec>>(vertices),
                                                        std::make_shared<std::vector<Prec>>(normals),
                                                        std::make_shared<std::vector<Prec>>(uvs),
                                                        std::make_shared<std::vector<int>>(indices),
                                                        nullptr, nullptr );
    }

    TEST(shapes_test, triangle_mesh)
    {
        std::vector<Prec> vertices = {0,0,0, 0,0,0, 0,0,0, 1,1,1, 1,1,1, 1,1,1, 2,2,2,2,2,2,2,2,2,2,2,2};
        std::vector<Prec> normals = {0,0,0, 1,1,1, 2,2,2};
        std::vector<Prec> uvs= {0,0,0, 1,1,1, 2,2,2};
        std::vector<int> indices= {0,0,0, 3,3,3, 6,6,6};

        auto ptr_vertices = std::make_shared<std::vector<Prec>>(vertices);
        auto ptr_normals = std::make_shared<std::vector<Prec>>(normals);
        auto ptr_uvs = std::make_shared<std::vector<Prec>>(uvs);
        auto ptr_indices = std::make_shared<std::vector<int>>(indices);

        std::shared_ptr<TriangleMesh> mesh = std::make_shared<TriangleMesh>(ptr_vertices,
                                                        ptr_normals,
                                                        ptr_uvs,
                                                        ptr_indices,
                                                        nullptr, nullptr );

        for (int i = 0; i < 9; i++)
        {
            EXPECT_EQ((*mesh->Vertices)[i], vertices[i]);
            EXPECT_EQ((*mesh->Normals)[i], normals[i]);
            EXPECT_EQ((*mesh->UVs)[i], uvs[i]);
            EXPECT_EQ((*mesh->Indices)[i], indices[i]);
        }

        auto triangles = *mesh->GenerateTriangles();

        auto bounds = triangles[0]->WorldBoundary();
        
        EXPECT_EQ(bounds.Max.x(), 2);
        EXPECT_EQ(bounds.Min.x(), 0);
        EXPECT_EQ(bounds.Max.y(), 2);
        EXPECT_EQ(bounds.Min.y(), 0);
        EXPECT_EQ(bounds.Max.z(), 2);
        EXPECT_EQ(bounds.Min.z(), 0);
    }

    // TEST(shapes_test, triangle_bounds_test)
    // {
    //     std::vector<float> vertices {1.f,2.f,3.f,4.f,5.f,6.f,7.f,8.f,9.f};
    //     auto mesh = get_mesh(vertices.data());

    //     Triangle_Indices idxs;

    //     idxs.vertex_1_idx = 0;
    //     idxs.vertex_2_idx = 3;
    //     idxs.vertex_3_idx = 6;

    //     Triangle triangle(mesh, idxs);

    //     Bounds3D<Prec> comp(7, 1, 8, 2, 9, 3);

    //     auto bound_out = triangle.WorldBoundary();

    //     EXPECT_EQ(comp.XMax, bound_out.XMax);
    //     EXPECT_EQ(comp.YMax, bound_out.YMax);
    //     EXPECT_EQ(comp.ZMax, bound_out.ZMax);

    //     EXPECT_EQ(comp.XMin, bound_out.XMin);
    //     EXPECT_EQ(comp.YMin, bound_out.YMin);
    //     EXPECT_EQ(comp.ZMin, bound_out.ZMin);
    // }

    // TEST(shapes_test, triangle_is_intersecting_test)
    // {
    //     std::vector<float> vertices {1,1,1, 1,2,1, 2,1,1};
    //     auto mesh = get_mesh(vertices.data());

    //     Triangle_Indices idxs;

    //     idxs.vertex_1_idx = 0;
    //     idxs.vertex_2_idx = 3;
    //     idxs.vertex_3_idx = 6;

    //     Triangle triangle(mesh, idxs);

    //     Ray ray({0,0,0}, {1,1.5,1});

    //    EXPECT_TRUE(triangle.IsIntersecting(ray));
    //}
}