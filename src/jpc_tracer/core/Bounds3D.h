#pragma once
#include"Base.h"
#include "Linalg.h"
#include "Ray.h"

#include <optional>

#include <iostream>

namespace jpc_tracer
{
    template<typename T>
    class Bounds3D
    {
    public:
        Bounds3D(T x_max,T x_min,T y_max,T y_min,T z_max,T z_min );
        Bounds3D(Eigen::Matrix<T, 3, 1> vec1, Eigen::Matrix<T, 3, 1> vec2);
        Bounds3D() = default;

        Vec3 Max;
        Vec3 Min;

        T Size() const;

        inline const Eigen::Matrix<T, 3, 1> &operator[](int i) const;
        inline const Eigen::Matrix<T, 3, 1> &operator[](int i);

        T MaximumExtent() const;
        int MaximumExtentDim() const;

        void Union(const Bounds3D<T>& b2);

        bool IsIntersecting(const Ray& ray, std::optional<Prec>& min_distance, const Eigen::Matrix<T, 3, 1>& inverse_direction, const int dir_is_negative[3]) const;
    };

    template<typename T>
    Bounds3D<T>::Bounds3D(T x_max,T x_min,T y_max,T y_min,T z_max,T z_min) 
        : Max(x_max, y_max, z_max), Min(x_min, y_min, z_min)
    {}

    template<typename T>
    inline Bounds3D<T>::Bounds3D(Eigen::Matrix<T, 3, 1> vec1, Eigen::Matrix<T, 3, 1> vec2)
        : Max(std::max(vec1(0,0), vec2(0, 0)), std::max(vec1(1, 0), vec2(1, 0)), std::max(vec1(2, 0), vec2(2, 0))),
        Min(std::min(vec1(0, 0), vec2(0, 0)), std::min(vec1(1, 0), vec2(1, 0)), std::min(vec1(2, 0), vec2(2, 0)))
    {
    }
    
    template<typename T>
    T Bounds3D<T>::Size() const
    {
        return (Max.x()-Min.x())*(Max.y()-Min.y())*(Max.z()-Min.y());
    }
    
    template<typename T>
    inline const Eigen::Matrix<T, 3, 1>& Bounds3D<T>::operator[](int i) 
    {
        return (i == 0) ? Min : Max;
    }
    
    template<typename T>
    inline const Eigen::Matrix<T, 3, 1>& Bounds3D<T>::operator[](int i) const
    {
        return (i == 0) ? Min : Max;
    }

    template<typename T>
    T Bounds3D<T>::MaximumExtent() const
    {
        return std::max({abs(Max.x()-Min.x()), 
                         abs(Max.y()-Min.y()),
                         abs(Max.z()-Min.y()) });
    }

    template<typename T>
    int Bounds3D<T>::MaximumExtentDim() const
    {
        const T max = MaximumExtent();

        if(abs(Max.x() - Min.x()) == max)
        {
            return 0;
        }
        else if(abs(Max.y() - Min.y()) == max)
        {
            return 1;
        }
        return 2;
    }
    
    template<typename T>
    void Bounds3D<T>::Union(const Bounds3D<T>& b2) 
    {
        Max[0] = std::max(Max[0], b2.Max[0]);
        Max[1] = std::max(Max[1], b2.Max[1]);
        Max[2] = std::max(Max[2], b2.Max[2]);

        Min[0] = std::min(Min[0], b2.Min[0]);
        Min[1] = std::min(Min[1], b2.Min[1]);
        Min[2] = std::min(Min[2], b2.Min[2]);
    }
    
    template<typename T>
    bool Bounds3D<T>::IsIntersecting(const Ray& ray, std::optional<Prec>& min_distance, const Eigen::Matrix<T, 3, 1>& inverse_direction, const int dir_is_negative[3]) const
    {
        const Bounds3D<T> &bound = *this;
        
        Prec x_min = (bound[    dir_is_negative[0]].x() - ray.Origin.x()) * inverse_direction.x();
        Prec x_max = (bound[1 - dir_is_negative[0]].x() - ray.Origin.x()) * inverse_direction.x();

        Prec y_min = (bound[    dir_is_negative[1]].y() - ray.Origin.y()) * inverse_direction.y();
        Prec y_max = (bound[1 - dir_is_negative[1]].y() - ray.Origin.y()) * inverse_direction.y();

        Prec z_min = (bound[    dir_is_negative[2]].z() - ray.Origin.z()) * inverse_direction.z();
        Prec z_max = (bound[1 - dir_is_negative[2]].z() - ray.Origin.z()) * inverse_direction.z();

        if(x_min > y_max || y_min > x_max)
            return false;

        if(x_min > z_max || z_min > x_max)
            return false;
            
        if(y_min > x_min)
            x_min = y_min;

        if(y_max < x_max)
            x_max = y_max;

        if(z_min > x_min)
           x_min = z_min;

        if(z_max < x_max)
            x_max = z_max;
        

        if(min_distance)
        {
            if(min_distance.value() <= x_min)
                return false;
        }

        if(x_max <= 0)
            return false;
        
        return true;
    }

    template<class T>
    Bounds3D<T> Union(const Bounds3D<T>& b1, const Bounds3D<T>& b2)
    {
        return Bounds3D<T> (std::max(b1.Max[0], b2.Max[0]),
                            std::min(b1.Min[0], b2.Min[0]),
                            std::max(b1.Max[1], b2.Max[1]),
                            std::min(b1.Min[1], b2.Min[1]),
                            std::max(b1.Max[2], b2.Max[2]),
                            std::min(b1.Min[2], b2.Min[2]) );
    }

    template<class T>
    Bounds3D<T> Union(const Bounds3D<T>& bound, const Vec3& vec) 
    {
        return Bounds3D<T> (std::max(bound.Max[0], vec.x()),
                            std::min(bound.Min[0], vec.x()),
                            std::max(bound.Max[1], vec.y()),
                            std::min(bound.Min[1], vec.y()),
                            std::max(bound.Max[2], vec.z()),
                            std::min(bound.Min[2], vec.z()) );
    }
}