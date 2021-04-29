#include "Lights.h"
#include "DiscreteSampler.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/engine/shadersystem/ShaderResults.h"
namespace jpctracer::shadersys
{
void Lights::AddPointLight(const Vec3& position, Spectrum color)
{
    point_lights.Add(position, color);
    compute_weights();
}

void Lights::AddDistanceLight(const Vec3& direction, Spectrum color)
{
    sun_lights.Add(direction, color);
    compute_weights();
}

template <class T> View<T> select_subrange(View<T> samples, const Range& range)
{
    return {samples.data + range.first, (size_t)range.last - range.first};
}

LightResults select_subrange(LightResults& result, const Range& range)
{
    return {select_subrange(result.emission, range), select_subrange(result.rays, range)};
}

const void Lights::Sample(View<Vec2> samples, const SurfaceInteraction& interaction, LightResults& result) const
{
    int result_size = 0;
    Range ranges[light_types];
    PartitionedSortedSamples(samples, {weights, light_types}, {ranges, light_types});
    LightResults sub_res = select_subrange(result, ranges[0]);
    point_lights.Sample(sub_res, select_subrange(samples, ranges[0]), interaction);
    result_size += sub_res.emission.size;

    uint remaining_begin = sub_res.emission.size;
    sub_res = select_subrange(result, {remaining_begin, remaining_begin + ranges[1].last - ranges[1].first});
    sun_lights.Sample(sub_res, select_subrange(samples, ranges[1]), interaction);
    result_size += sub_res.emission.size;

    result.emission.size = result_size;
    result.rays.size = result_size;
}

void Lights::compute_weights()
{
    Prec p_size = point_lights.Size();
    Prec s_size = sun_lights.Size();
    Prec all = p_size + s_size;
    weights[0] = p_size / all;
    weights[1] = s_size / all;
}
} // namespace jpctracer::shadersys