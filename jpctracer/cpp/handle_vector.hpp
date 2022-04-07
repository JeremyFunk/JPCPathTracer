#pragma once
#include "algorithms.hpp"
#include "iterator_adapter.h"
#include <cassert>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <limits>
#include <stack>
#include <vector>

namespace jpc
{

template <typename T, typename Info = void, typename Index = std::uint32_t>
struct handle
{
    Index id = 0;
    Info  info;

    handle() = default;
    handle(Index id, const T& value) : id(id), info(info)
    {
    }
};

template <typename T, typename Index> struct handle<T, void, Index>
{
    Index id = 0;
    handle() = default;
    handle(Index id, const T& value) : id(id)
    {
    }
};

template <typename T,
          typename Info = void,
          std::unsigned_integral Index = std::uint32_t>
class handle_vector
{
  private:
    std::vector<T>     m_data;
    std::vector<Index> m_free_ids;
    T                  m_default;

  public:
    class iterator_basic
    {
      public:
        using value_type = T;
        using pointer = T*;
        using difference_type =
            typename std::iterator_traits<T*>::difference_type;
        using reference = T&;
        using iterator_category = std::random_access_iterator_tag;

        struct state_t
        {

            Index id = 0;
            Index free_ids_i = 0;
            T*    data = nullptr;
            bool  operator==(const state_t s) const
            {
                return s.id == id;
            }
        };
        state_t s;
        Index*  free_ids = nullptr;
        Index   free_ids_n = 0;

        iterator_basic() = default;
        iterator_basic(Index* free_ids,
                       Index  free_ids_n,
                       Index  free_ids_start,
                       Index  id,
                       T*     data)
            : free_ids(free_ids),
              free_ids_n(free_ids_n), s{id, free_ids_start, data}
        {
        }
        void increment()
        {
            ++s.id;
            ++s.data;
            while (s.free_ids_i < free_ids_n && s.id == free_ids[s.free_ids_i])
            {
                ++s.free_ids_i;
                ++s.data;
                ++s.id;
            }
        }
        void decrement()
        {
            --s.id;
            --s.data;
            while (s.id == free_ids[s.free_ids_i])
            {
                --s.data;
                --s.id;
                if (s.free_ids_i > 0)
                    --s.free_ids_i;
            }
        }
        void increment(difference_type n)
        {
            if (n > 0)
            {
                s.id += n;
                s.data += n;
                while (s.free_ids_i < free_ids_n
                       && s.id >= free_ids[s.free_ids_i])
                {
                    s.free_ids_i++;
                    s.id++;
                    s.data++;
                }
            }
            else
            {
                s.id += n;
                s.data += n;
                while (s.free_ids_i > 0 && s.id <= free_ids[s.free_ids_i - 1])
                {
                    --s.id;
                    --s.data;
                    --s.free_ids_i;
                }
            }
        }

        difference_type difference(const iterator_basic& b) const
        {
            return s.data - b.s.data + difference_type(b.s.free_ids_i)
                   - difference_type(s.free_ids_i);
        }

        T* ptr() const
        {
            return s.data;
        }

        const state_t& state() const
        {
            return s;
        }

        state_t& state()
        {
            return s;
        }
    };

    using handle_t = handle<T, Info, Index>;

    using iterator = adapter::iterator<iterator_basic>;
    handle_vector() = default;

    handle_vector(const T& default_element) : m_default(default_element)
    {
    }

    handle_t push_back(const T& value)
    {
        if (m_free_ids.empty())
        {
            m_data.push_back(value);
            assert(m_data.size() < std::numeric_limits<Index>::max());
            Index id(m_data.size()-1) ;
            return handle_t{id,m_data[id]};
        }
        else
        {
            auto id = m_free_ids.back();
            m_data[id] = value;
            m_free_ids.pop_back();
            return handle_t{id,m_data[id]};
        }
    }

    T& operator[](const handle_t& id)
    {
        return m_data[id.id];
    }

    const T& operator[](const handle_t& id) const
    {
        return m_data[id.id];
    }

    bool contains(const handle_t& id) const
    {
        if (id.id < m_data.size())
        {
            auto res = std::find(m_free_ids.begin(), m_free_ids.end(), id.id);
            return res != m_free_ids.begin();
        }
    };

    void reserve(const Index& n)
    {
        m_data.reserve(n);
    }

    void erase(const handle_t& id)
    {
        operator[](id) = m_default;
        m_free_ids.push_back(id.id);
        algorithms::linear_insert(m_free_ids.begin(), m_free_ids.end() - 1);
    }

    Index capacity() const
    {
        return m_data.capacity();
    }

    Index size() const
    {
        return m_data.size();
    }

    Index count() const
    {
        return m_data.size() - m_free_ids.size();
    }

    T* data()
    {
        return m_data.data();
    }

    const T* data() const
    {
        return m_data.data();
    }

    T& default_element()
    {
        return m_default;
    }

    iterator begin()
    {
        return iterator(iterator_basic(
            m_free_ids.data(), m_free_ids.size(), 0, 0, m_data.data()));
    }
    iterator end()
    {
        return iterator(iterator_basic(m_free_ids.data(),
                                       m_free_ids.size(),
                                       m_free_ids.size(),
                                       m_data.size(),
                                       m_data.data() + m_data.size()));
    }
};
}; // namespace jpc
