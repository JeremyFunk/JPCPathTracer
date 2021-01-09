#pragma once
#include "core/IScene.h"
#include "core/IShape.h"
#include "core/ILight.h"
#include "core/Spectrum.h"
#include "shapes/Sphere.h"
#include "lights/DirectionalLight.h"
#include "lights/PointLight.h"

namespace scenes
{
    using namespace core;
    class BasicScene : public IScene
    {
    public:
        BasicScene();
        virtual std::optional<SurfaceProperties> Intersect(const Ray& ray) const override;
        virtual std::optional<Prec> IntersectionDistance(const Ray& ray) const override;
        virtual std::vector<std::shared_ptr<ILight>> GetLights() const override;

    private:
        std::shared_ptr<std::vector<std::shared_ptr<IShape>>> shapeList;
        std::vector<std::shared_ptr<ILight>> lightList;
    };
}