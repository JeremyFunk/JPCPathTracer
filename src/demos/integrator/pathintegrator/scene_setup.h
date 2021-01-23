#pragma once
#include "core/Linalg.h"
#include "core/IShape.h"
#include "core/ILight.h"
namespace jpc_tracer {

Ref<std::vector<Ref<IShape>>> generate_shapes();

Ref<std::vector<Ref<ILight>>> generate_lights();
}