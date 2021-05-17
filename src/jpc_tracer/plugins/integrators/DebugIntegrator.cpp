#include "DebugIntegrator.h"
#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include <vector>

namespace jpctracer
{
struct DebugPayload
{
    Vec3 scattering_dir;
    Vec3 light_dir;
    Prec light_distance = 1;
    Spectrum light_color = Black();
    Spectrum bsdf_color = FromRGB({1, 0, 1});
    Prec bsdf_pdf = 0;
};
void DebugIntegrator::Integrate(UInt2 pixel, const ICamera* camera, ISampler* sampler, Tracer& tracer,
                                film::Film& film) const
{
    Ray ray = camera->Generate(pixel, {0.5, 0.5});
    DebugPayload payload;
    tracer(DebugBehavior(), ray, &payload);
    film.SavePixel("ScatteringRay", pixel, Vec3ToSpec(payload.scattering_dir));
    Prec length = payload.scattering_dir.norm();
    Spectrum val = std::abs(length - 1) < 0.001 ? FromValue(1) : FromValue(0);
    film.SavePixel("ScatteringRayIs1", pixel, val);
    film.SavePixel("ScatteringRayZ", pixel, FromValue(payload.scattering_dir[2] * 0.5 + 0.5));
    film.SavePixel("LightRay", pixel, Vec3ToSpec(payload.light_dir));
    film.SavePixel("LightDistance", pixel, FromValue(std::log(payload.light_distance) / 10));
    film.SavePixel("LightColor", pixel, payload.light_color);
    film.SavePixel("Bsdf", pixel, payload.bsdf_color);
    film.SavePixel("BsdfPdf", pixel, FromValue(payload.bsdf_pdf));
}

void DebugBehavior::ClosestHitProgram(const HitPoint& hit_point, DebugPayload* payload, Tracer& trace) const
{
    Vec2 sample = {0.5, 0.5};

    auto light_res = hit_point.ActiveLights({&sample, 1});
    auto results = hit_point.Shader(light_res.rays, {&sample, 1});

    payload->light_dir = light_res.rays[0].Direction;
    payload->scattering_dir = results.sampled_rays[1].Direction;
    payload->bsdf_color = results.eval_bsdf[0].value;
    payload->bsdf_pdf = results.eval_bsdf[0].pdf;
    payload->light_distance = light_res.rays[0].ClipEnd;
    payload->light_color = light_res.emission[0].value;
}

void DebugBehavior::Miss(const Spectrum& background_color, DebugPayload* payload) const
{
    payload->scattering_dir = {-1, -1, -1};
}
} // namespace jpctracer