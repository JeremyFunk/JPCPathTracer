#pragma once
#include<optional>
#include <vector>
#include "Ray.h"
#include "ILight.h"
#include "SurfaceProperties.h"

namespace core {


    class IScene
    {
    public:

        virtual std::optional<SurfaceProperties> Intersect(const Ray& ray) const = 0;
        virtual std::optional<Prec> IntersectionDistance(const Ray& ray) const = 0;
        virtual std::vector<std::shared_ptr<ILight>> GetLights() const = 0;
    };
}