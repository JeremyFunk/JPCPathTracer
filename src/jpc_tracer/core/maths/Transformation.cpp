#include "Transformation.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/engine/raytracing/Base.h"

namespace jpctracer
{
Transformation Transformation::dot(const Transformation& trans)
{
    return {this->Matrix.dot(trans.Matrix), trans.InverseMatrix.dot(this->InverseMatrix)};
}

Prec degree_to_rad(Prec degree)
{
    return degree * M_PI / 180;
}
// clang-format off
Transformation Transformation::Identity() 
{
    Mat4x4 identity = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
    return Transformation{identity,identity};
}



Mat4x4 __XRotMat4x4(Prec x)
{
    return {
        1, 0          ,   0          , 0,
        0, std::cos(x), - std::sin(x), 0,
        0, std::sin(x),   std::cos(x), 0,
        0, 0          ,   0          , 1
    };
}

Mat4x4 __YRotMat4x4(Prec x)
{
    return {
            std::cos(x) , 0, std::sin(x), 0,
            0           , 1, 0          , 0,
        - std::sin(x) , 0, std::cos(x), 0,
            0           , 0,   0        , 1
    };
}

Mat4x4 __ZRotMat4x4(Prec x)
{
    return {
            std::cos(x), -std::sin(x), 0, 0,
            std::sin(x),  std::cos(x), 0, 0,
            0          ,  0          , 1, 0,
            0          ,  0          , 0, 1
    };
}
Transformation Scale(Prec x,Prec y,Prec z) 
{
    return Transformation{
        Mat4x4{
            x, 0, 0, 0,
            0, y, 0, 0,
            0, 0, z, 0,
            0, 0, 0, 1
        },
        Mat4x4{
            1.f/x, 0    , 0    , 0,
            0    , 1.f/y, 0    , 0,
            0    , 0    , 1.f/z, 0,
            0    , 0    , 0    , 1
        }          
    };
}

Transformation Translation(Prec x,Prec y,Prec z) 
{
    return Transformation{
        Mat4x4{
            1, 0, 0, x,
            0, 1, 0, y,
            0, 0, 1, z,
            0, 0, 0, 1
        },
        Mat4x4{
            1, 0, 0, -x,
            0, 1, 0, -y,
            0, 0, 1, -z,
            0, 0, 0,  1
        }
    };
}
// clang-format on 
Transformation XRotation(Prec degree) 
{
    Prec rad = degree_to_rad(degree);
    return {__XRotMat4x4(rad),__XRotMat4x4(-rad)};
}

Transformation YRotation(Prec degree) 
{
    Prec rad = degree_to_rad(degree);
    return {__YRotMat4x4(rad),__YRotMat4x4(-rad)};
}

Transformation ZRotation(Prec degree) 
{
    Prec rad = degree_to_rad(degree);
    return {__ZRotMat4x4(rad),__ZRotMat4x4(-rad)};
}

Transformation XYZRotation(Prec x_deg, Prec y_deg, Prec z_deg) 
{
    return ZRotation(x_deg).dot(YRotation(y_deg)).dot(XRotation(z_deg));
}



Transformation TransScalRot(Vec3 translation, Vec3 scale, Vec3 rotation) 
{
    return Translation(translation).dot(Scale(scale)).dot(XYZRotation(rotation));
}

Transformation TransScalRot(Vec3 translation, Prec scale, Vec3 rotation) 
{
    return TransScalRot(translation,Vec3{scale,scale,scale},rotation);
}


Vec3 ApplyMat4x4(const Mat4x4 &mat, const Vec3 &vec)
{
math::Vec<Prec, 4> result =
    mat.dot(math::Vec<Prec, 4>{vec[0], vec[1], vec[2], 1});
return Vec3{result[0], result[1], result[2]};

}

Transformation Translation(Vec3 translation) 
{
    return Translation(translation[0],translation[1],translation[2]);
}

Transformation Scale(Vec3 scales) 
{
    return Scale(scales[0],scales[1],scales[2]);
}

Transformation XYZRotation(Vec3 degrees) 
{
    return XYZRotation(degrees[0],degrees[1],degrees[2]);
}

Norm3 ApplyMat4x4(const Mat4x4 &mat, const Norm3 &vec)
{
    math::Vec<Prec, 4> result = mat.dot(math::Vec<Prec, 4>{vec[0], vec[1], vec[2], 0});
    auto test = Vec3{result[0], result[1], result[2]};

    return test.normalized();

}

Vec3 TransformTo(const Transformation& trans, const Vec3& vec)
{
    return ApplyMat4x4(trans.Matrix,vec);
}

Vec3 TransformBack(const Transformation &trans, const Vec3 &vec) 
{
    return ApplyMat4x4(trans.InverseMatrix,vec);
}; 

Norm3 TransformTo(const Transformation& trans, const Norm3& vec) 
{
    return ApplyMat4x4(trans.Matrix,vec);
};

Norm3 AllpyInverse(const Transformation &trans, const Norm3 &vec) 
{
    return ApplyMat4x4(trans.InverseMatrix,vec);
}; 

Ray TransformTo(const Transformation& trans, const Ray& ray) 
{
    return {TransformTo(trans,ray.Direction), TransformTo(trans,ray.Origin), ray.LensPosition, ray.ClipEnd, ray.Time};
}

Ray TransformBack(const Transformation& trans, const Ray& ray) 
{
    return {AllpyInverse(trans,ray.Direction), TransformBack(trans,ray.Origin), ray.LensPosition, ray.ClipEnd, ray.Time};
}


Bounds3D TransformTo( const Transformation& trans, const Bounds3D& bounds)
{
    const auto vec1 = TransformTo(trans, bounds.Min);
    const auto vec2 = TransformTo(trans, bounds.Max);

    const Vec3 min = {std::min(vec1[0], vec2[0]),
                        std::min(vec1[1], vec2[1]),
                        std::min(vec1[2], vec2[2])};

    const Vec3 max = {std::max(vec1[0], vec2[0]),
                        std::max(vec1[1], vec2[1]),
                        std::max(vec1[2], vec2[2])};

    return Bounds3D(min, max);
}

Transformation CreateNormalSpace(const Norm3& normal, const Vec3& point)
{

    const Norm3& n = normal;
    Vec3 temp_v = {0,0,-1};
    if (n[2] != 0)
    {
        temp_v = {0,-1,0};
    }
    auto t1 = n.cross(temp_v).normalized();
    auto t2 = n.cross(t1).normalized();

    // clang-format off
    Transformation normal_dir = {
        {
            t1[0],t1[1],t1[2],0,
            t2[0],t2[1],t2[2],0,
            n[0] ,n[1] ,n[2], 0,
            0    ,0    ,0   , 1
        },
        {
            t1[0],t2[0],n[0],0,
            t1[1],t2[1],n[1],0,
            t1[2],t2[2],n[2],0,
            0    ,0    ,0   ,1
        }
    };
    //clang-format on
    return normal_dir.dot(Translation(point*-1));
}
}
