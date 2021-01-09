#pragma once
#include "Eigen/Dense"
#include "Eigen/src/Core/Matrix.h"
#include "constants.h"

namespace core {
    using Prec = float;
    using Vec4 = Eigen::Vector4f;
    using Vec3 = Eigen::Vector3f;
    using Vec2 = Eigen::Vector2f;
    using Vec2i = Eigen::Vector2i;
    using Vec3i = Eigen::Vector3i;

    using Mat3x3 = Eigen::Matrix<Prec, 3,3>;
    using Mat4x4 = Eigen::Matrix<Prec, 4,4>;
    



    class LinAlg{

        
    public:
        static Mat4x4 GetTranslationMatrix(Vec3 translation){
            auto mat = Mat4x4(Mat4x4::Identity());
            mat(0,3) = translation[0];
            mat(1,3) = translation[1];
            mat(2,3) = translation[2];

            return mat;
        }

        static Mat4x4 GetScaleMatrix(Vec3 scale){
            auto mat = Mat4x4(Mat4x4::Identity());
            mat(0,0) = scale[0];
            mat(1,1) = scale[1];
            mat(2,2) = scale[2];
            mat(3,3) = 1;

            return mat;
        }




        static Mat4x4 GetRotationMatrixX(Prec x){
            Prec cosX = cos((x / 180) * M_PI);
            Prec sinX = sin((x / 180) * M_PI);

            auto mat = Mat4x4(Mat4x4::Identity());
            mat(0,0) = 1;
            mat(1,1) = cosX;
            mat(1,2) = sinX;
            mat(2,1) = -sinX;
            mat(2,2) = cosX;
            mat(3,3) = 1;
            
            return mat;
        }

        static Mat4x4 GetRotationMatrixY(Prec y){
            Prec cosY = cos((y / 180) * M_PI);
            Prec sinY = sin((y / 180) * M_PI);

            auto mat = Mat4x4(Mat4x4::Identity());
            mat(0,0) = cosY;
            mat(0,2) = -sinY;
            mat(1,1) = 1;
            mat(2,0) = sinY;
            mat(2,2) = cosY;
            mat(3,3) = 1;
            
            return mat;
        }

        static Mat4x4 GetRotationMatrixZ(Prec z){
            Prec cosZ = cos((z / 180) * M_PI);
            Prec sinZ = sin((z / 180) * M_PI);

            auto mat = Mat4x4(Mat4x4::Identity());
            mat(0,0) = cosZ;
            mat(0,1) = -sinZ;
            mat(1,0) = sinZ;
            mat(1,1) = cosZ;
            mat(2,2) = 1;
            mat(3,3) = 1;
            
            return mat;
        }



        static Mat4x4 GetRotationMatrix(Vec3 rotation){
            auto matX = GetRotationMatrixX(rotation[0]);
            auto matY = GetRotationMatrixY(rotation[1]);
            auto matZ = GetRotationMatrixZ(rotation[2]);

            return matX * matY * matZ;
        }





        
        static Mat4x4 GetTransformationMatrix(Vec3 position, Vec3 rotation, Vec3 scale){
            auto scaleM = GetScaleMatrix(scale);

            auto rotationM = GetRotationMatrix(rotation);
            auto translationM = GetTranslationMatrix(position);
            
            return translationM * rotationM * scaleM;
        }













        static Mat3x3 GetScaleMatrix3(Vec3 scale){
            auto mat = Mat3x3(Mat3x3::Identity());
            mat(0,0) = scale[0];
            mat(1,1) = scale[1];
            mat(2,2) = scale[2];
            return mat;
        }




        static Mat3x3 GetRotationMatrixX3(Prec x){
            Prec cosX = cos((x / 180) * M_PI);
            Prec sinX = sin((x / 180) * M_PI);

            auto mat = Mat3x3(Mat3x3::Identity());
            mat(0,0) = 1;
            mat(1,1) = cosX;
            mat(1,2) = sinX;
            mat(2,1) = -sinX;
            mat(2,2) = cosX;
            
            return mat;
        }

        static Mat3x3 GetRotationMatrixY3(Prec y){
            Prec cosY = cos((y / 180) * M_PI);
            Prec sinY = sin((y / 180) * M_PI);

            auto mat = Mat3x3(Mat3x3::Identity());
            mat(0,0) = cosY;
            mat(0,2) = -sinY;
            mat(1,1) = 1;
            mat(2,0) = sinY;
            mat(2,2) = cosY;
            
            return mat;
        }

        static Mat3x3 GetRotationMatrixZ3(Prec z){
            Prec cosZ = cos((z / 180) * M_PI);
            Prec sinZ = sin((z / 180) * M_PI);

            auto mat = Mat3x3(Mat3x3::Identity());
            mat(0,0) = cosZ;
            mat(0,1) = -sinZ;
            mat(1,0) = sinZ;
            mat(1,1) = cosZ;
            mat(2,2) = 1;
            
            return mat;
        }



        static Mat3x3 GetRotationMatrix3(Vec3 rotation){
            auto matX = GetRotationMatrixX3(rotation[0]);
            auto matY = GetRotationMatrixY3(rotation[1]);
            auto matZ = GetRotationMatrixZ3(rotation[2]);

            return matX * matY * matZ;
        }





        
        static Mat3x3 GetTransformationMatrix3(Vec3 rotation, Vec3 scale){
            auto scaleM = GetScaleMatrix3(scale);
            auto rotationM = GetRotationMatrix3(rotation);
            
            return rotationM * scaleM;
        }






        static Vec3 Interpolate(Vec3 v1, Vec3 v2, Vec3 v3, Prec u, Prec v){
            return u * v2 + v * v3 + (1 - u - v) * v1;
        }
        static Vec2 Interpolate(Vec2 v1, Vec2 v2, Vec2 v3, Prec u, Prec v){
            return u * v2 + v * v3 + (1 - u - v) * v1;
        }
    };
}