#include "JPCRenderer.h"
#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/engine/films/Film.h"
#include "jpc_tracer/engine/raytracing/Geometry.h"
#include "jpc_tracer/engine/raytracing/detail/Scene.h"
#include "jpc_tracer/engine/renderer/Tracer.h"
#include "jpc_tracer/engine/shadersystem/Lights.h"
#include <cstdint>
#include <memory>
#include <vector>

namespace jpctracer::renderer
{
JPCRenderer::JPCRenderer(std::unique_ptr<ISampler>&& sampler, std::unique_ptr<ICamera>&& camera,
                         std::unique_ptr<IIntegrator>&& integrator)
    : m_sampler(std::move(sampler)), m_camera(std::move(camera)), m_integrator(std::move(integrator))
{
}

void JPCRenderer::Draw(std::shared_ptr<Geometry> geomtry)
{
    raytracing::MeshId mesh_id;
    switch (geomtry->mesh.index())
    {

    case 0: {
        mesh_id = m_scene_builder.AddMesh(std::move(std::get<raytracing::TriangleMesh>(geomtry->mesh)));
        geomtry->mesh = mesh_id;
    }
    break;
    case 1:
        mesh_id = m_scene_builder.AddMesh(std::move(std::get<raytracing::SphereMesh>(geomtry->mesh)));
        geomtry->mesh = mesh_id;
        break;
    case 3:
        mesh_id = std::get<raytracing::MeshId>(geomtry->mesh);
        break;
    }

    auto instance_id = m_scene_builder.AddInstance(mesh_id, geomtry->transformation);
    for (auto materials : geomtry->MaterialSlots)
        m_scene_builder.MaterialBind(instance_id, materials.first, materials.second.Id);
    if (geomtry->MaterialSlots.size() == 0)
        m_scene_builder.MaterialBind(instance_id, 0, 0);
}

struct Tile
{
    UInt2 XRange;
    UInt2 YRange;
};

std::vector<Tile> SeperateToTiles(uint width, uint height, uint tile_size)
{
    std::vector<Tile> tiles;
    std::vector<UInt2> tile_xranges;
    std::vector<UInt2> tile_yranges;

    for (uint x = 0; x < width; x += tile_size)
        tile_xranges.push_back({x, std::min<uint>(x + tile_size, width)});

    for (uint y = 0; y < height; y += tile_size)
        tile_yranges.push_back({y, std::min<uint>(y + tile_size, height)});

    for (auto yrange : tile_yranges)
        for (auto xrange : tile_xranges)
            tiles.push_back({xrange, yrange});
    return tiles;
}

void JPCRenderer::Render(uint width, uint height, std::string directory)
{
    m_camera->SetWidthHeight(width, height);
    film::Film result_film(width, height);

    std::vector<Tile> tiles = SeperateToTiles(width, height, TileSize);

    const ShaderBuffer& buffer = MaterialLib.GetBuffer();
    const std::unique_ptr<raytracing::Scene> scene = m_scene_builder.Build(Acceleration);
    const shadersys::Lights* lights = &LightsLib;
    JPC_LOG_INFO("Start Rendering");
    if (ShouldMultiThread)
    {
#pragma omp parallel for
        for (int i = 0; i < tiles.size(); i++)
        {
            Tile tile = tiles[i];
            ISampler* thread_sampler(m_sampler->Clone());
            static thread_local shadersys::ShaderResultsStack shaderres{};
            //#pragma omp threadprivate(shaderres)
            jpc_assertm(shaderres.IsEmpty(), "ShaderResultsStack is not in default state");

            Tracer tracer(buffer, scene.get(), lights, shaderres);
            for (uint y = tile.YRange[0]; y < tile.YRange[1]; y++)
            {
                for (uint x = tile.XRange[0]; x < tile.XRange[1]; x++)
                {
                    UInt2 pixel = {x, y};
                    m_integrator->Integrate(pixel, m_camera.get(), thread_sampler, tracer, result_film);
                }
            }
            free(thread_sampler);
        }
    }
    else
    {
        shadersys::ShaderResultsStack shaderres;
        jpc_assertm(shaderres.IsEmpty(), "ShaderResultsStack is not in default state");

        for (int i = 0; i < tiles.size(); i++)
        {
            Tile tile = tiles[i];
            ISampler* thread_sampler(m_sampler->Clone());
            Tracer tracer(buffer, scene.get(), lights, shaderres);
            for (uint y = tile.YRange[0]; y < tile.YRange[1]; y++)
            {
                for (uint x = tile.XRange[0]; x < tile.XRange[1]; x++)
                {
                    UInt2 pixel = {x, y};
                    // if (x == 54 && y == 32)
                    // {
                    //     JPC_LOG_INFO("Debug");
                    // }
                    m_integrator->Integrate(pixel, m_camera.get(), thread_sampler, tracer, result_film);
                }
            }
            free(thread_sampler);
        }
    }
    JPC_LOG_INFO("End Rendering");

    result_film.WriteChannels(directory);
}
} // namespace jpctracer::renderer