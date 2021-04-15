// #pragma once
// #include "jpc_tracer/core/maths/maths.h"
// #include "jpc_tracer/engine/raytracing/detail/intersection/acceleration/IntersectionInfo.h"
// #include "jpc_tracer/engine/raytracing/SurfaceInteraction.h"
// #include "jpc_tracer/engine/raytracing/detail/intersection/acceleration/BoundingBoxIntersection.h"
// #include <optional>

// namespace jpctracer::raytracing
// {
//     template<class funcT, class StackIterator>
//     IntersectionResult BVHIntersection(StackIterator stack_iter, const Ray& ray, const funcT& IntersectShape)
//     {
//         //Acceleration for Box Intersect
//         Vec3 inv_direction {1 / ray.Direction[0], 1 / ray.Direction[1], 1 / ray.Direction[2]};
//         Int3 dir_is_negative {inv_direction[0] < 0, inv_direction[1] < 0, inv_direction[1] < 0};

//         auto start_iter = stack_iter - 1;

//         std::optional<SurfaceInteraction> closest_interaction;

//         while (stack_iter != start_iter)
//         {
//             bool is_intersecting = BoundIntersect(*stack_iter->Bounds(), ray, inv_direction, dir_is_negative);

//             if(is_intersecting)
//             {
//                 if (*stack_iter->IsChild())
//                 {
//                     IntersectionResult intersect_result = IntersectShape(*stack_iter, ray);

//                     if (intersect_result.ShouldTerminate) return intersect_result;

//                     closest_interaction = ClosestInteraction(closest_interaction, intersect_result.interaction);
//                 }
//                 else
//                 {
//                     for(const auto& child : *stack_iter->childs) *(stack_iter++) = child;
//                 }
//             }

//             stack_iter--;
//         }

//         return {closest_interaction, false};
//     }
// }