#pragma once
#include "core/IScene.h"
#include "core/IShape.h"
#include "core/ILight.h"
#include "core/Spectrum.h"
#include "shapes/Sphere.h"
#include "lights/DirectionalLight.h"
#include "lights/PointLight.h"
#include "accelerators/BVHAccel.h"
#include <memory>

namespace jpc_tracer
{

    class BVHScene : public IScene
    {
    public:
        BVHScene(const Ref<std::vector<Ref<IShape>>>& shapeList, const Ref<std::vector<Ref<ILight>>>& light);
        virtual std::optional<SurfaceProperties> Intersect(const Ray& ray) const override;
        virtual std::optional<Prec> IntersectionDistance(const Ray& ray) const override;
        
        virtual std::vector<Ref<ILight>> GetLights() const override;

        virtual BsdfMemoryInfo GetBsdfInfo() const override;

    private:
        Ref<BVHAccel> _bvh_tree;
        Ref<std::vector<Ref<IShape>>> _shapeList;
        Ref<std::vector<Ref<ILight>>> _lightList;
    };
}