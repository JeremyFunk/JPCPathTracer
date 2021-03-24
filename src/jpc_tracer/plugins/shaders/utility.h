#pragma once
#include<tuple>

namespace jpctracer::shader::detail {
    template<class T1, class T2>
    struct combine_tuple_traits
    {
        using type = std::tuple<T1,T2>;
    };

    template<class... Args1, class... Args2>
    struct combine_tuple_traits<std::tuple<Args1...>, std::tuple<Args2...>>
    {
        using type = std::tuple<Args1...,Args2...>;
    };

    template<class T>
    class TD;
}