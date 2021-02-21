#include "objLoader.h"
#include "core/Linalg.h"
#include "core/IShape.h"
#include "core/Logger.h"
#include <vector>
#include <iostream>

namespace jpc_tracer
{
    std::vector<std::string> split(const std::string& str, const std::string& delim);


    Ref<TriangleMesh> LoadMesh(std::string path,Ref<IMaterial> material, Ref<Transformation> transformation, Ref<std::vector<Ref<IShape>>> shapes)
    {
        //auto m = LinAlg::GetRotationMatrix(Vec3(0, 0, 90));


        std::vector<Prec> vertices;
        std::vector<Prec> normals;
        std::vector<Prec> uvs;
        
        std::vector<int> indices;

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



                    Vec4 vec4 = transformation->L2W * Vec4(v1, v2, v3, 1);

                    vertices.push_back(vec4[0]);
                    vertices.push_back(vec4[1]);
                    vertices.push_back(vec4[2]);
                }else if(line.rfind("vn ",0)==0){
                    Prec v1 = std::stof(splitted[1]);
                    Prec v2 = std::stof(splitted[2]);
                    Prec v3 = std::stof(splitted[3]);

                    //JPC_LOG_INFO("B Normal: {},{},{}",v1,v2,v3);
                    Vec3 vec3 = (transformation->Rot2W * Vec3(v1, v2, v3)).normalized();

                    //JPC_LOG_INFO("A Normal: {},{},{}",vec3[0],vec3[1],vec3[2]);

                    normals.push_back(vec3[0]);
                    normals.push_back(vec3[1]);
                    normals.push_back(vec3[2]);

                    // Normalize debug
                    // Vec3 vec3 = Vec3(v1, v2, v3).normalized();

                    // normals.push_back(vec3[0]);
                    // normals.push_back(vec3[1]);
                    // normals.push_back(vec3[2]);
                }else if(line.rfind("vt ",0)==0){
                    Prec v1 = std::stof(splitted[1]);
                    Prec v2 = std::stof(splitted[2]);
    
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

        auto mesh = MakeRef<TriangleMesh>(MakeRef<std::vector<Prec>>(vertices), MakeRef<std::vector<Prec>>(normals), 
        MakeRef<std::vector<Prec>>(uvs), MakeRef<std::vector<int>>(indices), material, transformation);

        

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