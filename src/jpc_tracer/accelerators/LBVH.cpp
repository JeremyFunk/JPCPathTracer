#include "LBVH.h"
#include "core/Bounds3D.h"
#include <algorithm>
#include <omp.h>
#include <optional>
#include <stdint.h>
#include <xbit_ops.h>



namespace jpc_tracer
{
    LBVHAccel::LBVHAccel(Ref<std::vector<Ref<IShape>>> shapes)
        : _shapes(shapes), _shapes_code_info(shapes->size()), _tree()
    {
        BuildLBVH();
    }
    
    std::optional<IntersectionData> LBVHAccel::Traversal(const Ray& ray) const
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
                const int first_idx = _tree[current_idx].Min_Idx;
                const int last_idx = _tree[current_idx].Max_Idx;
                const int split_idx = _tree[current_idx].Split_Idx;

                //right Leaf
                if(last_idx == split_idx + 1)
                {
                    shape_intersection(split_idx+1, ray, closestInteraction);
                }
                else
                {
                    // add right node to visit
                    if (split_idx + 1 < _tree.size())
                    {
                        nodes_to_visit[to_visit++] = split_idx + 1;   
                    }
                    else if (split_idx + 1 < _shapes->size())
                    {
                        shape_intersection(split_idx+1, ray, closestInteraction);
                    }
                }

                //left Leaf
                if(first_idx == split_idx)
                {   
                    shape_intersection(split_idx, ray, closestInteraction);
                }
                else 
                {
                    // add left node to visit
                    nodes_to_visit[to_visit++] = split_idx;
                }

                if(to_visit == 0)
                    break;

                current_idx = nodes_to_visit[--to_visit];
                
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

    void LBVHAccel::BuildLBVH() 
    {
        //ShapesInfo build
        const int shapes_size = _shapes->size();

        generate_shapes_code(shapes_size);

        //sort _shapes_code_info by morton code
        sort_shapes_code();

        _tree.resize(shapes_size - 1);

        //build binary radix tree
        #pragma omp parallel for
        for (int first_idx = 0; first_idx < shapes_size - 1; first_idx++)
        {   
            //direction
            int direction = calculate_direction(first_idx);

            //upper range bound
            int max_range = upper_range_bound(first_idx, direction);

            //lower range bound
            int min_range = lower_range_bound(first_idx, direction, max_range);

            int last_idx = first_idx + min_range*direction;

            int min_idx= std::min(first_idx, last_idx);
            int max_idx= std::max(first_idx, last_idx);

            int split_index = calculate_split_index(min_idx, direction, max_idx, min_range);

            _tree[first_idx].Min_Idx = min_idx;
            _tree[first_idx].Max_Idx  = max_idx;
            _tree[first_idx].Split_Idx = split_index;
        }

        #pragma omp parallel for
        for (int i = 0; i < shapes_size - 1; i++)
        {
            // calculate bounding box

            int first_idx = _tree[i].Min_Idx;
            int last_idx = _tree[i].Max_Idx;
               
            _tree[i].Bounds = _shapes_code_info[first_idx].Bounds;

            while (++first_idx < last_idx)
            {
                _tree[i].Bounds.Union(_shapes_code_info[first_idx].Bounds);
            }
        }
                
    }
    
    void LBVHAccel::generate_shapes_code(const size_t& shapes_size) 
    {
        for (size_t i = 0; i < shapes_size; i++)
        {
            _shapes_code_info[i] = {i, (*_shapes)[i]->WorldBoundary(), EncodeMorton3((*_shapes)[i]->getCenter())};
        }
    }
    
    void LBVHAccel::sort_shapes_code() 
    {
        std::sort(_shapes_code_info.begin(), _shapes_code_info.end(), 
                [](const ShapeMortonInfo& a, const ShapeMortonInfo& b)
                {
                    return (a.morton_code < b.morton_code);
                } );
    }
    
    int LBVHAccel::calculate_direction(const int& i) 
    {
        int direction = 1;
        if (length_combined_leading_zeros(i, i+1) - length_combined_leading_zeros(i, i-1) < 0)
            direction = -1;

        return direction;
    }
    
    int LBVHAccel::upper_range_bound(const int& i, const int& direction) 
    {
        int min_bit_length_diff = length_combined_leading_zeros(i, i - direction);

        int max_search_range = 2;
        while (length_combined_leading_zeros(i, i + (max_search_range * direction)) > min_bit_length_diff)
        {
            max_search_range *= 2;
        }

        return max_search_range;
    }
    
    int LBVHAccel::lower_range_bound(const int& i, const int& direction, const int& max_range) 
    {
        int min_bit_length_diff = length_combined_leading_zeros(i, i - direction);

        int min_search_range = 0;
        for (int t = max_range/2; t >= 1; t /= 2) //binary search
        {
            if (length_combined_leading_zeros(i, i + (min_search_range + t) * direction) > min_bit_length_diff)
            {
                min_search_range += t;
            }
        }

        return min_search_range;
    }
    
    int LBVHAccel::calculate_split_index(const int& first_idx, const int& direction, const int& last_idx, const int& min_range) 
    {
        uint32_t first_code = _shapes_code_info[first_idx].morton_code;
        uint32_t last_code = _shapes_code_info[last_idx].morton_code;

        if (first_code == last_code)
            return (first_idx + last_idx) >> 1;

        // int common_prefix = 31 - std::log2(first_code ^ last_code);
        int common_prefix = __builtin_clz(first_code ^ last_code);

        int split = first_idx;
        int step = last_idx - first_idx;

        do
        {
            step = (step + 1) >> 1;
            int new_split = split + step;

            if(new_split < last_idx)
            {
                int split_prefix = length_combined_leading_zeros(first_idx, new_split);

                if (split_prefix > common_prefix)
                {
                    split = new_split;
                }
            }

        } while(step > 1);

        return split;
    }
    
    inline int LBVHAccel::length_combined_leading_zeros(const size_t& idx_a, const size_t& idx_b) const
    {
        //in range
        if (idx_b >= _shapes->size() || idx_b < 0)
            return -1;
        
        uint32_t morton_code_a = _shapes_code_info[idx_a].morton_code;
        uint32_t morton_code_b = _shapes_code_info[idx_b].morton_code;

        //uniquefiy
        if (morton_code_a == morton_code_b)
        {
            size_t temp_idx_a = idx_a;
            size_t temp_idx_b = idx_b;

            while (temp_idx_a)
            {
                morton_code_a *= 10;
                temp_idx_a /= 10;
            }

            while (temp_idx_b)
            {
                morton_code_b *= 10;
                temp_idx_b /= 10;
            }
            morton_code_a += idx_a;
            morton_code_b += idx_b;

            // morton_code_a = morton_code_a + idx_a;
            // morton_code_b = morton_code_b + idx_b;
        }

        uint32_t combined = morton_code_a ^ morton_code_b;
        
        if (combined == 0)
        {
            return 32U;
        }
        
        return __builtin_clz(combined);

        return 31 - std::log2(combined);
    }

    inline uint32_t LBVHAccel::EncodeMorton3(const Vec3& vec) const
    {
        uint32_t x = LeftShift3(std::min(std::max(vec.x() * 1024.0f, 0.0f), 1023.0f));
        uint32_t y = LeftShift3(std::min(std::max(vec.y() * 1024.0f, 0.0f), 1023.0f));
        uint32_t z = LeftShift3(std::min(std::max(vec.z() * 1024.0f, 0.0f), 1023.0f));

        return x * 4 + y * 2 + z;
    }
    
    inline uint32_t LBVHAccel::LeftShift3(uint32_t x) const 
    {
        x = (x * 0x00010001u) & 0xFF0000FFu;
        x = (x * 0x00000101u) & 0x0F00F00Fu;
        x = (x * 0x00000011u) & 0xC30C30C3u;
        x = (x * 0x00000005u) & 0x49249249u;
        return x;
    }
    
    void LBVHAccel::shape_intersection(const int& idx, const Ray& ray, std::optional<IntersectionData>& closestInteraction) const
    {
        // intersect leaf
        std::optional<IntersectionData> surfaceInteraction = (*_shapes)[_shapes_code_info[idx].Shape_Idx]->Intersect(ray);

        //min interaction
        if(surfaceInteraction)
        {
            if(!closestInteraction.has_value() || closestInteraction->Distance > surfaceInteraction->Distance)
            {
                closestInteraction = surfaceInteraction;
            }
        }
    }
}