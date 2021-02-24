#pragma once
#include "Scene.h"

namespace jpc_rt_core {
    void TraceRay(const Scene* scene,const Ray& ray, Payload* payload);
}