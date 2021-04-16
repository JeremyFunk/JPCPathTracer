#pragma once

#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/engine/shadersystem/BsdfNode.h"
#include "jpc_tracer/engine/utilities/ImageIO.h"
#include <unordered_map>
namespace jpctracer::renderer
{
    struct RGB
    {
        Vec3 data;
        operator Spectrum() const {return FromRGB(data);}
        operator Vec3() const {return data;} 
        operator Prec() const {return (data[0]+data[1]+data[2])/3.f;}
    };

    class Texture
    {
    public:
        Texture(const Vec3* pixels, uint width, uint height);
        //Texture(Texture&&) = delete;
        inline RGB operator()(shadersys::ShaderContext& ctx) const
        {
            return operator()(ctx,ctx.normal_space.Interaction.UV);
        }
        inline RGB operator()(shadersys::ShaderContext& ctx,Vec2 uv) const
        {
            //error
            auto[u,v] = uv.Data;
            //JPC_LOG_INFO("A UV: V: {}",v);
            uint x_lower = u*m_width;
            uint x_upper = ceil(u*m_width);
            uint y_lower = v*m_height;
            uint y_upper = ceil(v*m_height);

            x_lower = std::clamp<uint>(x_lower,0,m_width-1);
            x_upper = std::clamp<uint>(x_upper,0,m_width-1);
            y_lower = std::clamp<uint>(y_lower,0,m_height-1);
            y_upper = std::clamp<uint>(y_upper,0,m_height-1);

            Vec3 lower_lower = m_pixels[y_lower*m_width+x_lower];
            Vec3 lower_upper = m_pixels[y_upper*m_width+x_lower];
            Vec3 upper_lower = m_pixels[y_lower*m_width+x_upper];
            Vec3 upper_upper = m_pixels[y_upper*m_width+x_upper];
            //JPC_LOG_INFO("Coords: Y: {}",y_lower);
            
            auto result = (lower_lower+lower_upper+upper_lower+upper_upper)/4.f;
            //auto result = lower_lower;
            //if(abs(result[0]-result[1])>0.2 || abs(result[0]-result[2])>0.2)
            //    JPC_LOG_INFO("Different Color: {},{},{}",result[0],result[1],result[2]);
            return {result};
            
        }
        ~Texture()
        {

        }
    private:
        const Vec3* m_pixels;
        uint m_width;
        uint m_height;
    };
    class TextureBuffer
    {
    public:
        Texture Load(std::string filepath); 
        Texture DefaultTexture();

    private:
        std::unordered_map<std::string, std::unique_ptr<utility::Image>> m_textures;  

        Vec3 m_default_color = {1,0,1};

    };
}