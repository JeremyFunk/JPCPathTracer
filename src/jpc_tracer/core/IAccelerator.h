#pragma once
#include "IShape.h"
#include <optional>

namespace jpc_tracer {

    class IAccelerator
    {
    public:
        virtual ~IAccelerator() {};

        virtual std::optional<IntersectionData> Traversal(const Ray& ray) const = 0;
    };
}