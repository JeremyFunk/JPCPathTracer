#pragma once
#include "jpc_tracer/core/core.h"
#include<iterator>
#include <memory>
#include "jpc_tracer/engine/raytracing/Geometry.h"
namespace jpctracer {



    template<class Iterator>
        requires std::input_iterator<Iterator> 
        &&  (std::convertible_to<std::iterator_traits<Iterator>,Sphere>
        || std::convertible_to<std::iterator_traits<Iterator>,Triangle>)
    class IGeometryRange
    {
    public:
        virtual Iterator begin() = 0;
        virtual Iterator end() = 0;
        
        std::shared_ptr<IAnimated<Transformation>> Transformation;

    };
}