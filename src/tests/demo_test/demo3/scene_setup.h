#include "core/Linalg.h"
#include "core/IShape.h"
#include "core/ILight.h"


std::shared_ptr<std::vector<std::shared_ptr<core::IShape>>> generate_shapes();

std::shared_ptr<std::vector<std::shared_ptr<core::ILight>>> generate_lights();