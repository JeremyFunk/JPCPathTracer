
#pragma once
#include "SceneBuilder.h"

namespace jpctracer::raytracing {

    void TraceRay(const Ray& ray, Payload* payload, TracingContext* context);
}