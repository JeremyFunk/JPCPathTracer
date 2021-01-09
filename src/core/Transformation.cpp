#include "Transformation.h"


namespace core
{
    Transformation::Transformation(Vec3 position, Vec3 rotation, Vec3 scale) : Position(position), Rotation(rotation), Scale(scale), L2W( Mat4x4::Identity()), Rot2W(Mat3x3::Identity())
    {
        RecalculateMatrix();
    }

    Transformation::Transformation(Vec3 position) : Position(position), Rotation(Vec3(0,0,0)), Scale(Vec3(1,1,1))
    {
        RecalculateMatrix();
    }
    
    void Transformation::RecalculateMatrix() 
    {
        L2W = LinAlg::GetTransformationMatrix(Position, Rotation, Scale);
        Rot2W = LinAlg::GetRotationMatrix3(Rotation);
    }
}