#include "gtest/gtest.h"
#include <vector>
#include "core/IMaterial.h"
#include "core/IShape.h"
#include "core/Linalg.h"
#include "accelerators/BVHAccel.h"
#include "shapes/TriangleMesh.h"
#include "core/Ray.h"
#include "core/Spectrum.h"
#include "shapes/TriangleMesh.h"
#include "utilities/objLoader.h"
#include "materials/BasicMaterial.h"

namespace jpc_tracer
{
    TEST(accelerators_test, shity_bvh_build)
    {
        std::shared_ptr<IMaterial> material_white = std::make_shared<BasicMaterial>(
            std::make_shared<ColorValueVec3>(ColorValueVec3({1,1,1})),1);

        auto transformation = std::make_shared<Transformation>(Vec3(0, 0, -8), Vec3(0, 0, 0), Vec3(1,1,1));

        auto shapesList = std::make_shared<std::vector<std::shared_ptr<IShape>>>();

        std::string path = "C:\\Users\\stade\\Desktop\\Cube.obj";
        auto mesh = LoadMesh(path, material_white, transformation, shapesList);

        auto triangles = mesh->GenerateTriangles();

        auto shapeList = std::make_shared<std::vector<std::shared_ptr<IShape>>>();

        for( auto triangle : *triangles)
            shapeList->push_back(triangle);

        BVHAccel bvh(shapeList, 1);

        bvh.BuildBVH();
        
        // auto bounds_triangle = (*triangles)[0]->WorldBoundary();

        // auto mesh_bound = (*triangles)[0]->WorldBoundary();

        // for (int i = 0; i < triangles->size(); i++)
        //     mesh_bound = Union(mesh_bound, (*triangles)[i]->WorldBoundary());

        // auto first_Max = (*out)[0].Bounds.Max;
        // auto first_Min = (*out)[0].Bounds.Min;

        // EXPECT_EQ(first_Max.x() , mesh_bound.Max.x());
        // EXPECT_EQ(first_Max.y() , mesh_bound.Max.y());
        // EXPECT_EQ(first_Max.z() , mesh_bound.Max.z());
        // EXPECT_EQ(first_Min.x() , mesh_bound.Min.x());
        // EXPECT_EQ(first_Min.y() , mesh_bound.Min.y());
        // EXPECT_EQ(first_Min.z() , mesh_bound.Min.z());
    }

    TEST(accelerators_test, shity_bvh_intersect)
    {
        std::shared_ptr<IMaterial> material_white = std::make_shared<BasicMaterial>(
            std::make_shared<ColorValueVec3>(ColorValueVec3({1,1,1})),1);

        auto transformation = std::make_shared<Transformation>(Vec3(0, 0, -8), Vec3(0, 0, 0), Vec3(1,1,1));

        auto shapeList = std::make_shared<std::vector<std::shared_ptr<IShape>>>();

        std::string path = "E:\\dev\\pathTrace\\JPCPathTracer\\resource\\Susan.obj";
        auto mesh = LoadMesh(path, material_white, transformation, shapeList);

        // auto triangles = mesh->GenerateTriangles();

        // auto shapeList = std::make_shared<std::vector<std::shared_ptr<IShape>>>();

        // for( auto triangle : *triangles)
        //     shapeList->push_back(triangle);

        BVHAccel bvh(shapeList, 1);

        Ray ray({0,0,0},{0,0,-1});

        auto out = bvh.Traversal(ray);

        bool test = out.has_value();

        //EXPECT_TRUE(test);
    }
}