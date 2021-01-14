#include "objLoader.h"
#include "core/Linalg.h"
#include "core/IShape.h"
#include <vector>
#include <iostream>

namespace utilities
{
    std::vector<std::string> split(const std::string& str, const std::string& delim);


    std::shared_ptr<shapes::TriangleMesh> LoadMesh(std::string path,std::shared_ptr<core::IMaterial> material, std::shared_ptr<core::Transformation> transformation, std::shared_ptr<std::vector<std::shared_ptr<core::IShape>>> shapes)
    {
        //auto m = core::LinAlg::GetRotationMatrix(core::Vec3(0, 0, 90));


        std::vector<core::Prec> vertices;
        std::vector<core::Prec> normals;
        std::vector<core::Prec> uvs;
        
        std::vector<int> indices;

        std::string line;
        std::ifstream file;
        file.open(path);
        if(file.is_open()){
            while(std::getline(file, line)){
                auto splitted = split(line, " ");

                if(line.rfind("v ",0)==0){
                    core::Prec v1 = std::stof(splitted[1]);
                    core::Prec v2 = std::stof(splitted[2]);
                    core::Prec v3 = std::stof(splitted[3]);



                    core::Vec4 vec4 = transformation->L2W * core::Vec4(v1, v2, v3, 1);

                    vertices.push_back(vec4[0]);
                    vertices.push_back(vec4[1]);
                    vertices.push_back(vec4[2]);
                }else if(line.rfind("vn ",0)==0){
                    core::Prec v1 = std::stof(splitted[1]);
                    core::Prec v2 = std::stof(splitted[2]);
                    core::Prec v3 = std::stof(splitted[3]);

                    
                    core::Vec3 vec3 = (transformation->Rot2W * core::Vec3(v1, v2, v3)).normalized();

                    normals.push_back(v1);
                    normals.push_back(v2);
                    normals.push_back(v3);

                    // Normalize debug
                    // core::Vec3 vec3 = core::Vec3(v1, v2, v3).normalized();

                    // normals.push_back(vec3[0]);
                    // normals.push_back(vec3[1]);
                    // normals.push_back(vec3[2]);
                }else if(line.rfind("vt ",0)==0){
                    core::Prec v1 = std::stof(splitted[1]);
                    core::Prec v2 = std::stof(splitted[2]);
    
                    uvs.push_back(v1);
                    uvs.push_back(-v2);
                }

                if(line.rfind("f ",0)==0){
                    auto face1 = split(splitted[1], "/");
                    auto face2 = split(splitted[2], "/");
                    auto face3 = split(splitted[3], "/");

                    int v1 = std::stoi(face1[0]) - 1;
                    int u1 = std::stoi(face1[1]) - 1;
                    int n1 = std::stoi(face1[2]) - 1;

                    int v2 = std::stoi(face2[0]) - 1;
                    int u2 = std::stoi(face2[1]) - 1;
                    int n2 = std::stoi(face2[2]) - 1;

                    int v3 = std::stoi(face3[0]) - 1;
                    int u3 = std::stoi(face3[1]) - 1;
                    int n3 = std::stoi(face3[2]) - 1;

                    indices.push_back(v1);
                    indices.push_back(u1);
                    indices.push_back(n1);

                    indices.push_back(v2);
                    indices.push_back(u2);
                    indices.push_back(n2);  

                    indices.push_back(v3);
                    indices.push_back(u3);
                    indices.push_back(n3); 
                }
            }
            file.close();
        }else{
            std::cout << "Could not open the path: " << path << std::endl;
        }

        auto mesh = std::make_shared<shapes::TriangleMesh>(std::make_shared<std::vector<core::Prec>>(vertices), std::make_shared<std::vector<core::Prec>>(normals), 
        std::make_shared<std::vector<core::Prec>>(uvs), std::make_shared<std::vector<int>>(indices), material, transformation);

        

        auto triangleList = mesh->GenerateTriangles();

        for(auto triangle : *triangleList)
            shapes->push_back(triangle);

        return mesh;
    }



    std::vector<std::string> split(const std::string& str, const std::string& delim){
        std::vector<std::string> tokens;
        size_t prev = 0, pos = 0;
        do{
            pos = str.find(delim, prev);
            if(pos == std::string::npos) pos = str.length();
            std::string token = str.substr(prev, pos-prev);
            if(!token.empty()) tokens.push_back(token);
            prev = pos + delim.length();
        }
        while(pos < str.length() && prev <str.length());
        return tokens;
    }
}