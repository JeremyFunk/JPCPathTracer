#pragma once

namespace jpctracer {

    template<class T>
    auto Build(T t)
    {
        return t.Build();
    }

    template<class T, class Return>
    concept Builder = requires(T t)
    {
        {Build(t)}->std::convertible_to<Return>;
    };
}