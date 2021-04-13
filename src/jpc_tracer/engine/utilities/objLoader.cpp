#include "objLoader.h"
#include "stringhelper.h"
#include <vector>
#include <iostream>

namespace jpctracer::utility
{
   

    raytracing::TriangleMesh LoadMesh(std::string path)
    {
        //auto m = LinAlg::GetRotationMatrix(Vec3(0, 0, 90));



        raytracing::TriangleMesh mesh;
        std::vector<Vec3> normals;

        std::string line;
        std::ifstream file;
        file.open(path);
        if(file.is_open()){
            while(std::getline(file, line)){
                auto splitted = split(line, " ");

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
                    auto face1 = split(splitted[1], "/");
                    auto face2 = split(splitted[2], "/");
                    auto face3 = split(splitted[3], "/");

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


        return mesh;
    }


}