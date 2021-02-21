#include "BVHAccel.h"
#include "core/Bounds3D.h"
#include "core/Linalg.h"
#include "core/Logger.h"
#include <optional>
#include <vector>



namespace jpc_tracer
{
    BVHAccel::BVHAccel(Ref<std::vector<Ref<IShape>>> shapes, const int& max_shapes_in_leaf) 
        : _max_shapes_in_leaf(max_shapes_in_leaf), _shapes(shapes), _shapes_info(shapes->size()), _tree()
    {
        JPC_LOG_INFO("Building BVH");

        BuildBVH();

        JPC_LOG_INFO("BVH-Build finished");
    }

    BVHAccel::BVHAccel(Ref<std::vector<Ref<IShape>>> shapes) 
        : BVHAccel(shapes, 1)
    {}
   
    void BVHAccel::BuildBVH() 
    {
        //ShapesInfo build
        const int shapes_size = _shapes->size();

        //_shapes_info.resize(shapes_size);
        for (size_t i = 0; i < shapes_size; i++)
        {
            _shapes_info[i] = {i, (*_shapes)[i]->WorldBoundary()};
        }

        /*
        *   generate Recursive Tree
        */

        // max memory = size_of(Node) * 2 * number_shapes - 1
        int max_size = 2*shapes_size - 1;

        // linear tree
        // traversal: node, left, right
        _tree.reserve(max_size);
        
        // current pos
        int offset = 0;

        //build tree
        RecursiveBuild(0, shapes_size, offset);

        // clear unused memory
        _tree.shrink_to_fit();
    }

    void BVHAccel::RecursiveBuild(int start, int end, int& offset)
    {
        //overall bounding box
        Bounds3D<Prec> total_bound = _shapes_info[start].Bounds;
        for (int i = start + 1; i < end; i++)
        {
            total_bound = Union(total_bound, _shapes_info[i].Bounds);
        }

        int number_shapes = end - start;

        if(number_shapes == 1)
        {
            //only 1 shape

            //add leaf
            _tree.emplace_back(total_bound, number_shapes, start, 0);

            offset++;
        }
        else
        {
            //calculate children
            Bounds3D<Prec> center_bounds(_shapes_info[start].Center, _shapes_info[start+1].Center);
            for(int i = start+2; i < end; i++)
            {
                center_bounds = Union(center_bounds, _shapes_info[i].Center);
            }

            int maximum_extent = center_bounds.MaximumExtent();

            if(maximum_extent == 0)
            {
                //all objects have the same center

                //create leaf
                _tree.emplace_back(total_bound, number_shapes, start, 0);

                offset++;
            }
            else
            {
                //node creation
                int mid = (start + end) / 2;
                int dim = center_bounds.MaximumExtentDim();

                //Partion into equally sized subsets
                split_equal_subsets(dim, start, mid, end);

                int current_pos = offset;
                offset++;
                _tree.emplace_back(total_bound, number_shapes, start, 0);
                
                //First half
                RecursiveBuild(start, mid, /*allnodes,*/ offset);

                _tree[current_pos].Idx_Second_Child = offset;

                //second half
                RecursiveBuild(mid, end, /*allnodes,*/ offset);
            }            
        }

        return;
    }
    
    void BVHAccel::split_equal_subsets(const int& dim, const int& start, const int& mid, const int& end) 
    {
        //reorder _shapes_info in range start to end
        std::nth_element(&_shapes_info[start], &_shapes_info[mid], &_shapes_info[end-1]+1,
                            [dim](const ShapeInfo& a, const ShapeInfo& b)
                            {
                                return a.Center[dim] < b.Center[dim];
                            });
    }

    std::optional<IntersectionData> BVHAccel::Traversal(const Ray& ray) const
    {
        //precalculation for faster bounding box intersection
        Vec3 inv_direction {1 / ray.Direction.x(), 1 / ray.Direction.y(), 1 / ray.Direction.z()};
        int dir_is_neagative[3] = {inv_direction.x() < 0, inv_direction.y() < 0, inv_direction.z() < 0};

        //setup
        std::optional<IntersectionData> closestInteraction = std::nullopt;
        std::optional<Prec> intersection_distance = std::nullopt;

        //follow ray
        const int tree_size = _tree.size();

        //stack setup
        int to_visit = 0;
        int current_idx = 0;
        int nodes_to_visit[64];

        while(true)
        {   
            //update intersection distance
            if(closestInteraction)
                intersection_distance = std::make_optional<Prec>(closestInteraction->Distance);

            //Bounding Box is intersecting
            if (_tree[current_idx].Bounds.IsIntersecting(ray, intersection_distance ,inv_direction, dir_is_neagative))
            {
                const int triangle_start = _tree[current_idx].Idx_Shape_Start;

                //Intersection and saved Triangles
                if (triangle_start >= 0)
                { 
                    const int triangle_number = _tree[current_idx].Number_Shapes;

                    for (int i = triangle_start; i < triangle_start + triangle_number; i++)
                    {
                        std::optional<IntersectionData> surfaceInteraction = (*_shapes)[ _shapes_info[i].Shape_Idx]->Intersect(ray);

                        //min interaction
                        if(surfaceInteraction)
                        {
                            if(!closestInteraction.has_value() || closestInteraction->Distance > surfaceInteraction->Distance)
                            {
                                closestInteraction = surfaceInteraction;
                            }
                        }
                    }

                    if(to_visit == 0)
                        break;

                    current_idx = nodes_to_visit[--to_visit];
                } 
                else //intersection and to many shapes in node
                {
                     nodes_to_visit[to_visit++] = _tree[current_idx].Idx_Second_Child;
                     current_idx++;
                }
                
            }
            else //no intersection with Bounding Box
            {
                if (to_visit == 0)
                    break;

                current_idx = nodes_to_visit[--to_visit];
            }
        }

        return closestInteraction;
    }
}