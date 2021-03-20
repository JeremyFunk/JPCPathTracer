#pragma once
#include "maths/maths.h"

namespace jpctracer {
    template<class T>
    class IAnimated
    {
        virtual T Get(Prec time) const = 0;
        virtual T GetInverse(Prec time) const = 0;
        virtual T GetMin(Prec min_time, Prec max_time) const = 0;
        virtual T GetMax(Prec min_time, Prec max_time) const = 0;
    };
}