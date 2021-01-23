#pragma once
#include "Linalg.h"
#include <memory>
#ifdef _MSC_VER
    #include <vcruntime.h>
#endif
#include <array>
#include <vector>
#include "constants.h"
namespace jpc_tracer {
    class ISampler
    {
    public:
        //Is between 0 and 1
        virtual Prec Get1DSample() = 0;
        virtual Vec2 Get2DSample() = 0;
        //shape = {dim, sample_count}
        virtual void Get1DSampleArray(size_t dim, size_t sample_count, Prec* desination) = 0;
        //shape = {dim_y, dim_x, sample_count}
        virtual void Get2DSampleArray(size_t dim_y,size_t dim_x, size_t sample_count, Vec2* desination) = 0;
        virtual std::shared_ptr<ISampler> Clone() = 0;
    };
}