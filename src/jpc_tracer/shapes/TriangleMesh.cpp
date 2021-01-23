#include "TriangleMesh.h"
#include <memory>



namespace jpc_tracer
{
    TriangleMesh::TriangleMesh(Ref<std::vector<Prec>> vertices, Ref<std::vector<Prec>> normals, Ref<std::vector<Prec>> uvs, Ref<std::vector<int>> indices, Ref<IMaterial> material, Ref<::jpc_tracer::Transformation> transformation) 
    : Vertices(vertices), Normals(normals), UVs(uvs), Indices(indices), Material(material), Transformation(transformation)
    {
        
    }

    
    Ref<std::vector<Ref<Triangle>>> TriangleMesh::GenerateTriangles()
    {
        Ref<std::vector<Ref<Triangle>>> triangles = MakeRef<std::vector<Ref<Triangle>>>(Indices->size() / 9);

        for(int i = 0; i < Indices->size(); i+=9){
            (*triangles)[i/9] = MakeRef<Triangle>(Triangle(MakeRef<TriangleMesh>(*this), i));
        }

        return triangles;
    }
    
    std::vector<Ref<const IMaterial>> Triangle::GetMaterials() const 
    {
        return {_mesh->Material};
    }
}