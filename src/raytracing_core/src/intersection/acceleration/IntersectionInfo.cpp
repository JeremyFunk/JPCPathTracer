#include "IntersectionInfo.h"
#include <optional>

namespace jpc_rt_core {
    std::optional<SurfaceInteraction> ClosestInteraction(std::optional<SurfaceInteraction> interaction_1, std::optional<SurfaceInteraction> interaction_2)
    {
        if(interaction_1.has_value() && ! interaction_2.has_value())
        {
            return interaction_1;
        }
        else if (! interaction_1.has_value() && interaction_2.has_value()) {
            return interaction_2;
        }
        else if (interaction_1.has_value() && interaction_2.has_value()) {
            if(interaction_1->Distance > interaction_2->Distance)
                return interaction_2;
            return interaction_1;
        }
        return std::nullopt;
    }
}