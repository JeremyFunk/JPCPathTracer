#pragma once
#include"Linalg.h"
#include "Ray.h"
#include <optional>

namespace jpc_tracer {
    template<class T>
    class Bounds2D
    {
    public:
        Bounds2D(T x_max,T x_min,T y_max,T y_min );
        Bounds2D();
        
        T XMax;
        T XMin;
        T YMax;
        T YMin;

        T Size();
    };

    template<class T>
    Bounds2D<T>::Bounds2D(T x_max,T x_min,T y_max,T y_min) 
        : XMax(x_max), YMax(y_max), XMin(x_min), YMin(y_min)
    {           
    }
    
    template<class T>
    Bounds2D<T>::Bounds2D() 
        : XMax(0), YMax(0), XMin(0), YMin(0)
    {
        
    }
    
    template<class T>
    T Bounds2D<T>::Size() 
    {
        return (XMax-XMin)*(YMax-YMin);
    }
    
    
    // template<typename T>
    // class Bounds3D
    // {
    // public:
    //     Bounds3D(T x_max,T x_min,T y_max,T y_min,T z_max,T z_min );
    //     Bounds3D(Eigen::Matrix<T, 3, 1> vec1, Eigen::Matrix<T, 3, 1> vec2);
    //     Bounds3D() = default;

    //     // T XMax;
    //     // T XMin;
    //     // T YMax;
    //     // T YMin;
    //     // T ZMax;
    //     // T ZMin;

    //     Vec3 Max;
    //     Vec3 Min;

    //     T Size() const;

    //     inline const Eigen::Matrix<T, 3, 1> &operator[](int i) const;
    //     inline const Eigen::Matrix<T, 3, 1> &operator[](int i);

    //     T MaximumExtent() const;
    //     int MaximumExtentDim() const;

    //     bool IsIntersecting(const Ray& ray, std::optional<Prec>& min_distance, const Eigen::Matrix<T, 3, 1>& inverse_direction, const int dir_is_negative[3]) const;
    // };

    // template<typename T>
    // Bounds3D<T>::Bounds3D(T x_max,T x_min,T y_max,T y_min,T z_max,T z_min) 
    //     : Max(x_max, y_max, z_max), Min(x_min, y_min, z_min)
    //     //: XMax(x_max), YMax(y_max), ZMax(z_max), XMin(x_min), YMin(y_min), ZMin(z_min)
    // {}

    // template<typename T>
    // inline Bounds3D<T>::Bounds3D(Eigen::Matrix<T, 3, 1> vec1, Eigen::Matrix<T, 3, 1> vec2)
    //     : Max(std::max(vec1(0,0), vec2(0, 0)), std::max(vec1(1, 0), vec2(1, 0)), std::max(vec1(2, 0), vec2(2, 0))),
    //     Min(std::min(vec1(0, 0), vec2(0, 0)), std::min(vec1(1, 0), vec2(1, 0)), std::min(vec1(2, 0), vec2(2, 0)))
    // {
    // }
    
    // //template<typename T>
    // //Bounds3D<T>::Bounds3D() 
    // //    : Max(0,0,0), Min(0,0,0)
    // //    //: XMax(0), YMax(0), ZMax(0), XMin(0), YMin(0), ZMin(0)
    // //{}
    
    // template<typename T>
    // T Bounds3D<T>::Size() const
    // {
    //     return (Max.x()-Min.x())*(Max.y()-Min.y())*(Max.z()-Min.y());
    // }
    
    // template<typename T>
    // inline const Eigen::Matrix<T, 3, 1>& Bounds3D<T>::operator[](int i) 
    // {
    //     return (i == 0) ? Min : Max;
    // }
    
    // template<typename T>
    // inline const Eigen::Matrix<T, 3, 1>& Bounds3D<T>::operator[](int i) const
    // {
    //     return (i == 0) ? Min : Max;
    // }

    // template<typename T>
    // T Bounds3D<T>::MaximumExtent() const
    // {
    //     return std::max({Max.x()-Min.x(),Max.y()-Min.y(),Max.z()-Min.y()});
    // }

    // template<typename T>
    // int Bounds3D<T>::MaximumExtentDim() const
    // {
    //     const T max = std::max({Max.x()-Min.x(),Max.y()-Min.y(),Max.z()-Min.y()});

    //     if(Max.x() - Min.x() == max)
    //         return 0;
    //     else if(Max.y() - Min.y() == max)
    //         return 1;
    //     return 2;
    // }
    
    // template<typename T>
    // bool Bounds3D<T>::IsIntersecting(const Ray& ray, std::optional<Prec>& min_distance, const Eigen::Matrix<T, 3, 1>& inverse_direction, const int dir_is_negative[3]) const
    // {
    //     const Bounds3D<T> &bound = *this;
        
    //     Prec x_min = (bound[    dir_is_negative[0]].x() - ray.Origin.x()) * inverse_direction.x();
    //     Prec x_max = (bound[1 - dir_is_negative[0]].x() - ray.Origin.x()) * inverse_direction.x();

    //     Prec y_min = (bound[    dir_is_negative[1]].y() - ray.Origin.y()) * inverse_direction.y();
    //     Prec y_max = (bound[1 - dir_is_negative[1]].y() - ray.Origin.y()) * inverse_direction.y();

    //     Prec z_min = (bound[    dir_is_negative[2]].z() - ray.Origin.z()) * inverse_direction.z();
    //     Prec z_max = (bound[1 - dir_is_negative[2]].z() - ray.Origin.z()) * inverse_direction.z();

    //     if(x_min > y_max || y_min > x_max)
    //         return false;

    //     if(x_min > z_max || z_min > x_max)
    //         return false;

    //     if(y_min > x_min)
    //         x_min = y_min;

    //     if(y_max < x_max)
    //         x_max = y_max;

    //     if(z_min > x_min)
    //        x_min = z_min;

    //     if(z_max < x_max)
    //         x_max = z_max;

    //     if(min_distance)
    //     {
    //         if(min_distance.value() <= x_min)
    //             return false;
    //     }

    //     if(x_max <= 0)
    //         return false;
        
    //     return true;

    //     // Prec intersection1 = 0;
    //     // Prec intersection2 = 0;

    //     // for (int i = 0; i < 3; i++)
    //     // {
    //     //     Prec inv_ray_dir = 1 / ray.Direction[i];

    //     //     Prec near_solution = (Min[i] - ray.Origin[i]) * inv_ray_dir;
    //     //     Prec far_solution = (Max[i] - ray.Origin[i]) * inv_ray_dir;

    //     //     if(near_solution > far_solution)
    //     //         std::swap(near_solution, far_solution);
            
    //     //     intersection1 = near_solution > intersection1 ? near_solution : intersection1;

    //     //     if(intersection2 == 0)
    //     //         intersection2 = far_solution;
    //     //     else
    //     //         intersection2 = far_solution < intersection2 ? far_solution : intersection2;
            
    //     //     if(near_solution > far_solution)
    //     //         return false;
    //     // }
    //     // return true;
    // }

    // template<class T>
    // Bounds3D<T> Union(const Bounds3D<T>& b1, const Bounds3D<T>& b2)
    // {
    //     return Bounds3D<T> (std::max(b1.Max[0], b2.Max[0]),
    //                         std::min(b1.Min[0], b2.Min[0]),
    //                         std::max(b1.Max[1], b2.Max[1]),
    //                         std::min(b1.Min[1], b2.Min[1]),
    //                         std::max(b1.Max[2], b2.Max[2]),
    //                         std::min(b1.Min[2], b2.Min[2]) );
    // }

    // template<class T>
    // Bounds3D<T> Union(const Bounds3D<T>& bound, const Vec3& vec) 
    // {
    //     return Bounds3D<T> (std::max(bound.Max[0], vec.x()),
    //                         std::min(bound.Min[0], vec.x()),
    //                         std::max(bound.Max[1], vec.y()),
    //                         std::min(bound.Min[1], vec.y()),
    //                         std::max(bound.Max[2], vec.z()),
    //                         std::min(bound.Min[2], vec.z()) );
    // }
}