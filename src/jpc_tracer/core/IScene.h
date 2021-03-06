#pragma once
#include"Base.h"
#include<optional>
#include <vector>
#include "Ray.h"
#include "ILight.h"
#include "SurfaceProperties.h"
#include "Bsdf.h"

namespace jpc_tracer {


    class IScene
    {
    public:

        virtual std::optional<SurfaceProperties> Intersect(const Ray& ray) const = 0;
        virtual std::optional<Prec> IntersectionDistance(const Ray& ray) const = 0;
        virtual std::vector<Ref<ILight>> GetLights() const = 0;
        virtual BsdfMemoryInfo GetBsdfInfo() const = 0;
        virtual ~IScene(){}
    };
}