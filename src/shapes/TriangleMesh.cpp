#include "TriangleMesh.h"
#include <memory>



namespace shapes
{
    TriangleMesh::TriangleMesh(std::shared_ptr<std::vector<core::Prec>> vertices, std::shared_ptr<std::vector<core::Prec>> normals, std::shared_ptr<std::vector<core::Prec>> uvs, std::shared_ptr<std::vector<int>> indices, std::shared_ptr<core::IMaterial> material, std::shared_ptr<core::Transformation> transformation) 
    : Vertices(vertices), Normals(normals), UVs(uvs), Indices(indices), Material(material), Transformation(transformation)
    {
        
    }

    
    std::shared_ptr<std::vector<std::shared_ptr<Triangle>>> TriangleMesh::GenerateTriangles()
    {
        std::shared_ptr<std::vector<std::shared_ptr<Triangle>>> triangles = std::make_shared<std::vector<std::shared_ptr<Triangle>>>(Indices->size() / 9);

        for(int i = 0; i < Indices->size(); i+=9){
            (*triangles)[i/9] = std::make_shared<Triangle>(Triangle(std::make_shared<TriangleMesh>(*this), i));
        }

        return triangles;
    }
}