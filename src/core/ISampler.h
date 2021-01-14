#pragma once
#include "Linalg.h"
#include <memory>
#ifdef _MSC_VER
    #include <vcruntime.h>
#endif
#include <array>
#include <vector>
#include "constants.h"
namespace core {
    class ISampler
    {
    public:
        //Is between 0 and 1
        virtual Prec Get1DSample() = 0;
        virtual Vec2 Get2DSample() = 0;
        virtual std::unique_ptr<std::vector<Prec>> Get1DSampleArray(size_t size) = 0;
        virtual std::unique_ptr<std::vector<Vec2>> Get2DSampleArray(size_t size_x,size_t size_y) = 0;
        virtual std::shared_ptr<ISampler> Clone() = 0;
    };
}