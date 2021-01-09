#include "BVHAccel.h"
#include "core/Bounds3D.h"
#include "core/Linalg.h"
#include <optional>
#include <vector>



namespace accel
{
    BVHAccel::BVHAccel(std::shared_ptr<std::vector<std::shared_ptr<core::IShape>>> shapes, const int& max_shapes_in_leaf) 
        : _max_shapes_in_leaf(max_shapes_in_leaf), _shapes(shapes), _shapes_info(), _tree()
    {
        BuildBVH();
    }
   
    void BVHAccel::BuildBVH() 
    {
        //TriangleInfo build
        const int shapes_size = _shapes->size();

        _shapes_info.resize(shapes_size);
        for (size_t i = 0; i < shapes_size; i++)
        {
            _shapes_info[i] = {i, (*_shapes)[i]->WorldBoundary()};
        }

        //generate Recursive Tree
        //max memory = size_of(Node) * 2 * number_triangles - 1
        std::shared_ptr<std::vector<BVHNode>> allnodes = std::make_shared<std::vector<BVHNode>>(2*shapes_size - 1);
        //memory offset for all nodes
        int offset = 0;

        BVHNode* root = RecursiveBuild(0, shapes_size, allnodes, offset);


        //flatten Tree
        std::shared_ptr<std::vector<SmallBVHNode>> flattend = std::make_shared<std::vector<SmallBVHNode>>();
        int pos = linearise_tree(root, flattend);
        
        //return
        _tree = flattend;
    }
    
    std::optional<core::IntersectionData> BVHAccel::Traversal(const core::Ray& ray) const
    {
        //precalculation for faster bounding box intersection
        core::Vec3 inv_direction {1 / ray.Direction.x(), 1 / ray.Direction.y(), 1 / ray.Direction.z()};
        int dir_is_neagative[3] = {inv_direction.x() < 0, inv_direction.y() < 0, inv_direction.z() < 0};

        //setup
        std::optional<core::IntersectionData> closestInteraction = std::nullopt;
        std::optional<core::Prec> intersection_distance = std::nullopt;

        //follow ray
        const int tree_size = _tree->size();

        //stack setup
        int to_visit = 0;
        int current_idx = 0;
        int nodes_to_visit[64];

        while(true)
        {   
            //update intersection distance
            if(closestInteraction)
                intersection_distance = std::make_optional<core::Prec>(closestInteraction->Distance);

            //Bounding Box is intersecting
            if ((*_tree)[current_idx].Bounds.IsIntersecting(ray, intersection_distance ,inv_direction, dir_is_neagative))
            {
                const int triangle_start = (*_tree)[current_idx].Idx_Shape_Start;

                //Intersection and saved Triangles
                if (triangle_start >= 0)
                { 
                    const int triangle_number = (*_tree)[current_idx].Number_Shapes;

                    for (int i = triangle_start; i < triangle_start + triangle_number; i++)
                    {
                        std::optional<core::IntersectionData> surfaceInteraction = (*_shapes)[ _shapes_info[i].Shape_Idx]->Intersect(ray);

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
                     nodes_to_visit[to_visit++] = (*_tree)[current_idx].Idx_Second_Child;
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
    
    BVHNode* BVHAccel::RecursiveBuild(int start, int end, std::shared_ptr<std::vector<BVHNode>>& allnodes, int& offset)
    {
        BVHNode* node = allnodes->data() + offset;

        //overall bounding box
        core::Bounds3D<core::Prec> total_bound = _shapes_info[start].Bounds;
        for (int i = start + 1; i < end; i++)
        {
            total_bound = core::Union(total_bound, _shapes_info[i].Bounds);
        }

        int number_triangles = end - start;

        if(number_triangles == 1)
        {
            //add leaf
            offset++;
            node->InitLeaf(total_bound, number_triangles, start, end);
        }
        else
        {
            //calculate children
            core::Bounds3D<core::Prec> center_bounds(_shapes_info[start].Center, _shapes_info[start+1].Center);
            for(int i = start+2; i < end; i++)
            {
                center_bounds = core::Union(center_bounds, _shapes_info[i].Center);
            }

            int maximum_extent = center_bounds.MaximumExtent();

            if(maximum_extent == 0)
            {
                //all objects have the same center
                //create leaf
                offset++;
                node->InitLeaf(total_bound, number_triangles, start, end);
            }
            else
            {
                //node creation
                int mid = (start + end) / 2;
                int dim = center_bounds.MaximumExtentDim();

                //Partion into equally sized subsets
                split_equal_subsets(dim, start, mid, end);

                offset++;

                node->InitInner(total_bound, number_triangles, 
                            RecursiveBuild(start, mid, allnodes, offset), 
                            RecursiveBuild(mid, end, allnodes, offset));
            }            
        }

        return node;
        
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
    
    int BVHAccel::linearise_tree(BVHNode* node, std::shared_ptr<std::vector<SmallBVHNode>>& flattend)
    {   
        //node, left, right traversal

        if(node == nullptr)
            return -1;

        flattend->push_back({node->Bounds, node->Number_Shapes, node->Idx_Shape_Start});

        int pos = flattend->size() - 1;

        if(node->Number_Shapes > 1)
        {
            if(node->Idx_First_Child != nullptr)
                int pos_first = linearise_tree(node->Idx_First_Child, flattend);

            if (node->Idx_Second_Child != nullptr)
            {
                int pos_second = linearise_tree(node->Idx_Second_Child, flattend);

                (*flattend)[pos].Idx_Second_Child = pos_second;
            }
        }

        return pos;
    }
}