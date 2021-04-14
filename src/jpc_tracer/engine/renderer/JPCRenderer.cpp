#include "JPCRenderer.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/engine/PluginsApi.h"
#include "jpc_tracer/engine/films/Film.h"
#include "jpc_tracer/engine/raytracing/Geometry.h"
#include "jpc_tracer/engine/raytracing/detail/Scene.h"
#include "jpc_tracer/engine/renderer/Tracer.h"
#include <memory>
#include <vector>



namespace jpctracer::renderer
{
    void JPCRenderer::Draw(std::shared_ptr<Geometry> geomtry) 
    {
        raytracing::MeshId mesh_id;
        switch (geomtry->mesh.index()) {
            
            case 0:
                mesh_id = m_scene_builder.AddMesh(std::move(std::get<raytracing::TriangleMesh>(geomtry->mesh)));
                geomtry->mesh = mesh_id;
                break;
            case 1:
                mesh_id = m_scene_builder.AddMesh(std::move(std::get<raytracing::SphereMesh>(geomtry->mesh)));
                geomtry->mesh = mesh_id;
                break;
            case 3:
                mesh_id = std::get<raytracing::MeshId>(geomtry->mesh);
                break;
        }
        auto instance_id = m_scene_builder.AddInstance(mesh_id);
        for(auto materials: geomtry->MaterialSlots)
            m_scene_builder.MaterialBind(instance_id, materials.first, materials.second->material_id);
    }
    
    struct Tile
    {
        UInt2 XRange;
        UInt2 YRange;
    };

    std::vector<Tile> SeperateToTiles(uint width,uint height, uint tile_size)
    {
        std::vector<Tile> tiles;
        std::vector<UInt2> tile_xranges;
        std::vector<UInt2> tile_yranges;
        
        for(uint x = 0; x < width;x+=tile_size)
            tile_xranges.push_back({x,std::min<uint>(x+tile_size,width)});
        
        for(uint y = 0; y < height;y+=tile_size)
            tile_yranges.push_back({y,std::min<uint>(y+tile_size,height)});

        for(auto yrange : tile_yranges)
            for(auto xrange : tile_xranges)
                tiles.push_back({xrange,yrange});
        return tiles;
    }

    void JPCRenderer::Render(uint width,uint height,std::string directory) 
    {
        m_camera->SetWidthHeight(width, height);
        film::Film result_film(width,height);

        std::vector<Tile> tiles = SeperateToTiles(width,height, TileSize);

        const ShaderBuffer buffer = MaterialLib.CreateShaders();
        const std::unique_ptr<raytracing::Scene> scene = m_scene_builder.Build();

        #pragma omp parallel for
        for(Tile tile : tiles)
        {
            ISampler* thread_sampler(m_sampler->Clone());
            Tracer tracer(buffer,scene.get(),&LightsLib);
            for(uint y = tile.YRange[0];y<tile.YRange[1];y++)
            {
                for(uint x = tile.XRange[0];x<tile.XRange[1];x++)
                {
                    UInt2 pixel = {x,y};
                    m_integrator->Integrate(pixel, m_camera.get(),thread_sampler, tracer, result_film);
                }
            }
            free(thread_sampler);
        }

        result_film.WriteChannels(directory);
    }
}