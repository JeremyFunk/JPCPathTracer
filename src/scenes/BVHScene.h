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
        BVHScene(const std::shared_ptr<std::vector<std::shared_ptr<IShape>>>& shapeList, const std::shared_ptr<std::vector<std::shared_ptr<ILight>>>& light);
        virtual std::optional<SurfaceProperties> Intersect(const Ray& ray) const override;
        virtual std::optional<Prec> IntersectionDistance(const Ray& ray) const override;
        
        virtual std::vector<std::shared_ptr<ILight>> GetLights() const override;

        virtual BsdfMemoryInfo GetBsdfInfo() const override;

    private:
        std::shared_ptr<BVHAccel> _bvh_tree;
        std::shared_ptr<std::vector<std::shared_ptr<IShape>>> _shapeList;
        std::shared_ptr<std::vector<std::shared_ptr<ILight>>> _lightList;
    };
}