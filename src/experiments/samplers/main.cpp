#include "jpc_tracer/JPCTracer.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/plugins/samplers/GridSampler.h"
#include <fstream>
#include <iostream>
using namespace jpctracer;

int main()
{
    auto sampler = jpctracer::sampler::StratifiedSamplerFast();
    unsigned int n_x = 100;
    unsigned int n_y = 100;
    std::vector<jpctracer::Vec2> samples(n_x * n_y);

    sampler.Sample({n_x, n_y}, samples.data());

    std::ofstream file;
    file.open("samples.csv");
    file << "sample_x,sample_y\n";
    for (const auto& sample : samples)
    {
        file << sample[0] << "," << sample[1] << "\n";
    }
}