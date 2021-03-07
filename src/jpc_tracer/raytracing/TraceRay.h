
#pragma once
#include "Scene.h"

namespace jpctracer::raytracing {

    void TraceRay(const Scene* scene,const Ray& ray, Payload* payload, TracingContext* context);
}