#pragma once
#include "core/Linalg.h"
#include "core/IShape.h"
#include "core/ILight.h"
namespace jpc_tracer {

std::shared_ptr<std::vector<std::shared_ptr<IShape>>> generate_shapes();

std::shared_ptr<std::vector<std::shared_ptr<ILight>>> generate_lights();
}