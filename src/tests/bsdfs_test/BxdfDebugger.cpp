#include "BxdfDebugger.h"
#include "core/FilmTile.h"
#include "core/Linalg.h"
#include "films/BasicFilm.h"
#include <cmath>
#include <memory>
#include <vector>
#include "utilities/ImageIO.h"

namespace bsdfs
{
    DebugFilm::DebugFilm(int width, int height) 
        : _width(width), _height(height)
    {
        _pixels = std::vector<float>(_width*height*3);
        for(float& pixel : _pixels)
            pixel = 0;
        
    }

    void DebugFilm::AddValue(int x, int y, float value) 
    {
        x = std::min(x, _width - 1);
        y = std::min(y, _height - 1);
        _pixels[y*_width*3+x*3] += value;
        _pixels[y*_width*3+x*3+1] += value;
        _pixels[y*_width*3+x*3+2] += value;
        float p = _pixels[y * _width * 3 + x * 3];
        if(p >_max_val) _max_val = p;
    }
    
    void DebugFilm::AddColor(int x, int y, core::Spectrum spec) 
    {
        x = std::min(x, _width - 1);
        y = std::min(y, _height - 1);
        core::Vec3 rgb = spec.ToRGB();
        _pixels[y*_width*3+x*3] += rgb[0];
        _pixels[y*_width*3+x*3+1] += rgb[1];
        _pixels[y*_width*3+x*3+2] += rgb[2];

        float r = _pixels[y * _width * 3 + x * 3];
        float g = _pixels[y * _width * 3 + x * 3 + 1];
        float b = _pixels[y * _width * 3 + x * 3 + 2];
        _max_val = std::max<float>({ r,g,b,_max_val});
    }

    void DebugFilm::SetValue(int x, int y, float value)
    {
        x = std::min(x, _width - 1);
        y = std::min(y, _height - 1);
        _pixels[y * _width * 3 + x * 3] = value;
        _pixels[y * _width * 3 + x * 3 + 1] = value;
        _pixels[y * _width * 3 + x * 3 + 2] = value;

        _max_val = std::max<float>(value,_max_val);
    }
    
    void DebugFilm::SetColor(int x, int y, core::Spectrum spec)
    {
        x = std::min(x, _width - 1);
        y = std::min(y, _height - 1);
        core::Vec3 rgb = spec.ToRGB();
        _pixels[y * _width * 3 + x * 3] = rgb[0];
        _pixels[y * _width * 3 + x * 3 + 1] = rgb[1];
        _pixels[y * _width * 3 + x * 3 + 2] = rgb[2];

        float r = _pixels[y * _width * 3 + x * 3];
        float g = _pixels[y * _width * 3 + x * 3 + 1];
        float b = _pixels[y * _width * 3 + x * 3 + 2];
        _max_val = std::max<float>({ r,g,b,_max_val });

    }

    void DebugFilm::Save(std::string filepath)
    {
        auto conv_pixels = std::make_unique<std::vector<unsigned char>>(_width*_height*3);
        for(int i = 0 ;i < conv_pixels->size();i++)
        {
            conv_pixels->operator[](i) =_pixels[i] / _max_val * 255.0f;
        }
        utilities::WriteImage(filepath, conv_pixels->data(), _width, _height);
    }

    void DebugFilm::SaveLog(std::string filepath)
    {
        auto conv_pixels = std::make_unique<std::vector<unsigned char>>(_width * _height * 3);
        for (int i = 0; i < conv_pixels->size(); i++)
        {
            conv_pixels->operator[](i) = std::log(1 + _pixels[i]) * 255.0f;
        }
        utilities::WriteImage(filepath, conv_pixels->data(), _width, _height);
    }

    void BxdfDebugSampleUniform(IBXDF* bxdf, std::string name)
    {
        
        core::Vec3 in_vec;
        in_vec[0] = 0.5f;
        in_vec[1] = 0.5f;
        in_vec[2] = std::sqrt(0.5f);
        BxdfDebugSampleUniform(bxdf, name, in_vec);
    }

    void BxdfDebugSampleUniform(IBXDF* bxdf,std::string name, core::Vec3 income_vec)
    {
        int width = 100;
        int height = 100;
        DebugFilm* film_pdf = new DebugFilm(width,height);
        DebugFilm* film_values = new DebugFilm(width,height);
        DebugFilm* film_combined = new DebugFilm(width,height);
        DebugFilm* film_color = new DebugFilm(width,height);
        DebugFilm* film_dir = new DebugFilm(width, height);
        
        
        for(int y = 0; y < height;y++)
        {
            for(int x = 0; x < width;x++)
            {
                if (x == 64 && y == 40)
                {
                    std::cout<<std::endl;
                }
                float x_norm = (float) x / (float) width;
                float y_norm = (float) y / (float) height;
                auto result = bxdf->Evaluate(income_vec, {x_norm,y_norm});
                int x_mapped = result.OutgoingDirection[0] * (float) width / 2 + (float)width / 2;
                int y_mapped = result.OutgoingDirection[1] * (float) height /2 + (float)height / 2;
                
                core::Vec3 dir = result.OutgoingDirection;
                core::Vec3 dir_color = (dir * (core::Prec)0.5) + core::Vec3(0.5, 0.5, 0.5);
                if (result.OutgoingDirection[2] < 0)
                {
                    dir_color = { 0,0,1 };
                }
                if (result.OutgoingDirection[2] < 0.5)
                    continue;
                film_dir->AddColor(x, y, core::Spectrum::FromRGB(dir_color));
                film_pdf->AddValue(x, y, result.Pdf);
                //std::cout << "Pdf: " << result.Pdf << std::endl;
                film_values->AddValue(x_mapped, y_mapped, 1.f);
                film_color->SetColor(x_mapped, y_mapped, result.SurfaceColor);
                if (result.SurfaceColor.ToRGB()[0] > 0.1)
                {
                    std::cout << std::endl;
                }
                film_combined->AddValue(x_mapped, y_mapped, 1.f / result.Pdf);
               
                
            }
        }

        film_color->Save(name+"_color.png");
        film_combined->Save(name+"_combined.png");
        film_values->Save(name+"_values.png");
        film_pdf->Save(name+"_pdf.png");
        film_dir->Save(name + "_dir.png");

        film_color->SaveLog(name + "_color_log.png");
        film_combined->SaveLog(name + "_combined_log.png");
        film_values->SaveLog(name + "_values_log.png");
        film_pdf->SaveLog(name + "_pdf_log.png");
        film_dir->SaveLog(name + "_dir_log.png");

        delete film_pdf;
        delete film_values;
        delete film_combined;
        delete film_color;
        delete film_dir;

        

    }
    
    void DistDebugSampleUniform(IMicrofacetDistribution* dist, std::string name, core::Vec3 income_vec)
    {
        int width = 100;
        int height = 100;
        DebugFilm* film_dist = new DebugFilm(width,height);
        DebugFilm* film_sampled = new DebugFilm(width, height);
        for(int y = 0; y < height;y++)
        {
            for(int x = 0; x < width;x++)
            {
                float x_norm = (float) x / (float) width *2 - 1.f;
                float y_norm = (float) y / (float) height *2 - 1.f;
                std::cout << x_norm << "," << y_norm << std::endl;
                float z_norm = std::sqrt(1-x_norm*x_norm-y_norm*y_norm);
                core::Vec3 norm = {x_norm,y_norm,z_norm};
                film_dist->AddValue(x,y,dist->Distribution(norm));
                auto sample = dist->SampleFromDistribution(income_vec, { (float)x / (float)width,(float)y / (float)height });
                int x_mapped = sample[0] * (float)width / 2 + (float)width / 2;
                int y_mapped = sample[1] * (float)height / 2 + (float)height / 2;
                film_sampled->AddValue(x_mapped, y_mapped, 1);

            }
        }
        film_sampled->SaveLog(name + "_sampled_log.png");
        film_sampled->Save(name + "_sampled.png");
        film_dist->SaveLog(name+"_dist.png");
        delete film_dist;
        delete film_sampled;
        
    }
}