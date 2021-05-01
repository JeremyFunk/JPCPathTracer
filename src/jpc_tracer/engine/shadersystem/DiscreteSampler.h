#pragma once
#include "jpc_tracer/core/core.h"
#include <algorithm>
#include <array>

namespace jpctracer::shadersys
{

class DiscreteSampler
{
  public:
    // precondition weights.size>0
    DiscreteSampler(const Prec* weights, int size);

    struct Result
    {
        size_t SampledIdx;
        Prec ResampledPoint;
    };

    // random_val in [0,1]
    Result Sample(const Prec& random_val) const;

  private:
    std::vector<Prec> m_weights_sum;
};

struct Range
{
    int first;
    int last;
};

// the samples will be remapped so that it is uncorrelated to the ranges
void PartitionedSamples(View<Vec2> samples, View<const Prec> weights, View<Range> out_ranges);
void PartitionedSortedSamples(View<Vec2> samples, View<const Prec> weights, View<Range> out_ranges);

} // namespace jpctracer::shadersys