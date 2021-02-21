#pragma once
#include"Base.h"
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

}