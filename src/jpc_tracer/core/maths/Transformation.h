#pragma once
#include "Constants.h"


namespace jpctracer {

    struct Bounds3D
    {
        Vec3 Min;
        Vec3 Max;

        Bounds3D() : Min(Vec3()), Max(Vec3())
        {}

        Bounds3D(const Vec3& min, const Vec3& max) : Min(min), Max(max)
        {}

        Vec3 operator[](const int& i) const
        {
            return (i == 0) ? Min : Max;
        }

        Vec3& operator[](const int& i)
        {
            return (i == 0) ? Min : Max;
        }
    };

    struct Transformation
    {
        Mat4x4 Matrix;
        Mat4x4 InverseMatrix;

        static Transformation Identity();
        
        Transformation dot(const Transformation& trans);

    };

    Transformation XRotation(Prec degree);
    Transformation YRotation(Prec degree);
    Transformation ZRotation(Prec degree);

    Transformation XYZRotation(Prec x_deg, Prec y_deg, Prec z_deg);
    Transformation XYZRotation(Vec3 degrees);

    Transformation Scale(Prec x,Prec y,Prec z);
    Transformation Scale(Vec3 scales);
    

    Transformation Translation(Prec x,Prec y,Prec z); 
    Transformation Translation(Vec3 translation); 

    Transformation RotScalTrans(Vec3 translation, Vec3 scale, Vec3 rotation);
    

    Vec3 Apply(const Transformation &trans, const Vec3 &vec);
    Vec3 AllpyInverse(const Transformation &trans, const Vec3 &vec);

    Norm3 Apply(const Transformation& trans, const Norm3& vec);
    Norm3 ApplyInverse(const Transformation& trans, const Norm3& vec);
    

    Ray Apply(const Transformation& trans, const Ray& ray);
    
    Ray ApplyInverse(const Transformation& trans, const Ray& ray);
    


    Bounds3D Apply(const Transformation& trans,const Bounds3D& bounds);
    Bounds3D ApplyInverse(const Transformation& trans,const Bounds3D& bounds);
    


    
}