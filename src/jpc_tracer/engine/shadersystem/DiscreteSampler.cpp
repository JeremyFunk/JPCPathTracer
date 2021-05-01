#include "DiscreteSampler.h"

namespace jpctracer::shadersys
{
DiscreteSampler::Result DiscreteSampler::Sample(const Prec& random_val) const
{
    auto sampled_it = std::lower_bound(std::begin(m_weights_sum), std::end(m_weights_sum), random_val);

    Prec lower_sum = 0;
    if (sampled_it > std::begin(m_weights_sum))
        lower_sum = *(sampled_it - 1);
    Prec upper_sum = *sampled_it;

    Prec new_rand_val = (random_val - lower_sum) / (upper_sum - lower_sum);

    size_t sampled_idx = std::distance(std::begin(m_weights_sum), sampled_it);

    return {sampled_idx, new_rand_val};
}

void PartitionedSamples(View<Vec2> samples, View<const Prec> weights, View<Range> out_ranges)
{
    std::ranges::sort(samples, [](const Vec2& a, const Vec2& b) { return a[0] < b[0]; });
    PartitionedSortedSamples(samples, weights, out_ranges);
}

void PartitionedSortedSamples(View<Vec2> samples, View<const Prec> weights, View<Range> out_ranges)
{
    int smp_i = 0;
    int range_i = 0;

    Prec sum = weights[0];
    int weights_i = 1;

    out_ranges[0].first = range_i;
    for (smp_i = 0; smp_i < samples.size; smp_i++)
    {
        if (samples[smp_i][0] >= sum)
        {
            out_ranges[range_i].last = smp_i;
            range_i++;
            out_ranges[range_i].first = smp_i;
            sum += weights[weights_i];
            weights_i++;
        }
    }
    out_ranges[range_i].last = smp_i;
    range_i++;
    for (; range_i < out_ranges.size; range_i++)
    {
        out_ranges[range_i].first = smp_i;
        out_ranges[range_i].last = smp_i;
    }
}
DiscreteSampler::DiscreteSampler(const Prec* weights, int size) : m_weights_sum(size, 0)
{
    assert(size > 0);
    m_weights_sum[0] = weights[0];
    for (int i = 1; i < size; i++)
    {
        m_weights_sum[i - 1] += weights[i];
    }
}
} // namespace jpctracer::shadersys