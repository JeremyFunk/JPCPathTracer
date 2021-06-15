#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Vec.h"
#include <benchmark/benchmark.h>
#include <stdint.h>

static void Vec_add(benchmark::State& state)
{
    jpctracer::Vec3 a{0.5, 1.0 / 3.0, 0.25};
    jpctracer::Vec3 b = a;

    for (auto _ : state)
        a += b;

    return;
}

static void Vec_add_comp(benchmark::State& state)
{
    jpctracer::Prec a[3] = {0.5, 1.0 / 3.0, 0.25};
    jpctracer::Prec b[3] = {a[0], a[1], a[2]};

    for (auto _ : state)
        for (auto i = 0; i < 3; i++)
            a[i] += b[i];

    return;
}

// BENCHMARK(Vec_add)->UseRealTime();
// BENCHMARK(Vec_add_comp)->UseRealTime();