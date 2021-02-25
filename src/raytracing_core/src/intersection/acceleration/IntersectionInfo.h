#pragma once
#include "raytracing_core/include/Base.h"
#include "raytracing_core/include/SurfaceInteraction.h"
#include <optional>

namespace jpc_rt_core
{
    struct IntersectionInfo
    {
        Vec2 UV;
        float Distance;
    };


    template<class KeyT>
    std::pair<std::optional<IntersectionInfo>,KeyT> ClosestIntersection(std::optional<IntersectionInfo> interaction_1,KeyT idx_1, std::optional<IntersectionInfo> interaction_2,KeyT idx_2)
    {
        if(interaction_1.has_value() && ! interaction_2.has_value())
        {
            return {interaction_1,idx_1};
        }
        else if (! interaction_1.has_value() && interaction_2.has_value()) {
            return {interaction_2,idx_2};
        }
        else if (interaction_1.has_value() && interaction_2.has_value()) {
            if(interaction_1->Distance > interaction_2->Distance)
                return {interaction_2, idx_1};
            return {interaction_1,idx_2};
        }
        return {std::nullopt,idx_1};
    }


    std::optional<SurfaceInteraction> ClosestInteraction(std::optional<SurfaceInteraction> interaction_1, std::optional<SurfaceInteraction> interaction_2);

    struct IntersectionResult
    {
        std::optional<SurfaceInteraction> interaction;
        bool ShouldTerminate;
    };
}