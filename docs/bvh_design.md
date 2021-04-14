# BVH Design

## Traversal

```cpp
    struct IntersectionResult;
    struct SurfaceInteraction;


    struct Node
    {
        std::array<int, 2> childs;
        bool is_child();
        Bound bounds();
    };

    struct Node2
    {
        std::array<void*, 2> childs;
        bool is_child();
        Bound bounds();
    }

    StackIterator
    {
        const Node* operator*() const
        {
            return NodeIterator{_tree, _stack[_pos]};
        }

        StackIterator& operator++()
        {
            _pos++;
            return *this;
        }

        StackIterator& operator--()
        {
            _pos--;
            return *this;
        }

        bool operator!=(const StackIterator& comp)
        {
            return _pos != comp._pos;
        }

        Node* _tree;
        std::shared_ptr<std::array<int, 64>>> _stack;
        int _pos;
    }

    NodeIterator
    {
        const Node& operator*() const
        {
            return *(_tree + _pos);
        }

        void operator=(int i)
        {
            _pos = i;
        }

        Node* _tree;
        int _pos;
    }

    template<class funcT, class StackIterator>
    IntersectionResult BvhTraversalIntersect(StackIterator stack_iter, const Ray& ray, const funcT& IntersectShape)
    {
        auto start_iter = stack_iter - 1;

        std::optional<SurfaceInteraction> closest;

        while (stack_iter != start_iter)
        {
            auto result = BoundIntersect(*stack_iter->bounds(), ray);

            if(result.IsHit())
            {
                if (*stack_iter.is_child)
                {
                    IntersectionResult intersect_result = IntersectShape(ray, *stack_iter);

                    if (intersect_result.ShouldTerminate) return intersect_result;

                    closest = ClosestInteraction(closest, intersect_result.interaction);
                }
                else
                {
                    for(const auto& child : *reverse(stack_iter->childs)) *(stack_iter++) = child;
                }
            }

            stack_iter--;
        }

        return {closest_interaction, false};
    }

```