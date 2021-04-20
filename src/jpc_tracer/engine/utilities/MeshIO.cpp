#include "MeshIO.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/engine/JPCTracerApi.h"
#include "jpc_tracer/engine/renderer/Geomtry.h"
#include "stringhelper.h"
#include <memory>
#include <vector>
#include <iostream>

namespace jpctracer
{
   

    std::shared_ptr<renderer::Geometry> LoadMesh(std::string path)
    {
        //auto m = LinAlg::GetRotationMatrix(Vec3(0, 0, 90));



        raytracing::TriangleMesh mesh;
        std::vector<Vec3> normals;

        std::string line;
        std::ifstream file;
        file.open(path);
        if(file.is_open()){
            while(std::getline(file, line)){
                auto splitted = utility::split(line, " ");

                if(line.rfind("v ",0)==0){
                    Prec v1 = std::stof(splitted[1]);
                    Prec v2 = std::stof(splitted[2]);
                    Prec v3 = std::stof(splitted[3]);

                    mesh.Vertices.push_back({v1,v2,v3});

                }else if(line.rfind("vn ",0)==0){
                    Prec v1 = std::stof(splitted[1]);
                    Prec v2 = std::stof(splitted[2]);
                    Prec v3 = std::stof(splitted[3]);


                    mesh.Normals.push_back({v1,v2,v3});

                }else if(line.rfind("vt ",0)==0){
                    Prec v1 = std::stof(splitted[1]);
                    Prec v2 = std::stof(splitted[2]);
    
                    mesh.UVs.push_back({v1,v2});
                }

                if(line.rfind("f ",0)==0){
                    auto face1 = utility::split(splitted[1], "/");
                    auto face2 = utility::split(splitted[2], "/");
                    auto face3 = utility::split(splitted[3], "/");

                    uint v1 = std::stoi(face1[0]) - 1;
                    uint u1 = std::stoi(face1[1]) - 1;
                    uint n1 = std::stoi(face1[2]) - 1;

                    uint v2 = std::stoi(face2[0]) - 1;
                    uint u2 = std::stoi(face2[1]) - 1;
                    uint n2 = std::stoi(face2[2]) - 1;

                    uint v3 = std::stoi(face3[0]) - 1;
                    uint u3 = std::stoi(face3[1]) - 1;
                    uint n3 = std::stoi(face3[2]) - 1;

                    mesh.TriangleGeometries.push_back({v1,v2,v3});
                    mesh.TriangleShadings.push_back({{u1,u2,u3},{n1,n2,n3},0});


                    
                }
            }
            file.close();
        }else{
            std::cout << "Could not open the path: " << path << std::endl;
        }

        return std::make_shared<Geometry>(std::move(mesh));
    }
    
    std::shared_ptr<renderer::Geometry> CreateTriangle(Vec3 vertex1,Vec3 vertex2, Vec3 vertex3) 
    {
        raytracing::TriangleMesh mesh;
        mesh.Vertices.push_back(vertex1);
        mesh.Vertices.push_back(vertex2);
        mesh.Vertices.push_back(vertex3);
        mesh.UVs.push_back({0,0});
        mesh.UVs.push_back({1,0});
        mesh.UVs.push_back({1,1});
        Vec3 sup1 = vertex2-vertex1;
        Vec3 sup2 = vertex3-vertex1;
        Vec3 normal = sup1.cross(sup2);
        mesh.Normals.push_back(normal.normalized());
        mesh.TriangleGeometries.push_back({0,1,2});
        mesh.TriangleShadings.push_back({{0,0,0},{0,1,2},0});
        return std::make_shared<Geometry>(std::move(mesh));      
        
    }
    
    std::shared_ptr<renderer::Geometry> CreateSphere(Vec3 Pos, Prec radius) 
    {
        raytracing::SphereMesh mesh;
        mesh.Spheres.push_back({Pos, radius});
        mesh.MaterialSlots.resize(1);
        return std::make_shared<Geometry>(std::move(mesh));
    }


}