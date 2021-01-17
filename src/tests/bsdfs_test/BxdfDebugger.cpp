#include "BxdfDebugger.h"
#include "core/FilmTile.h"
#include "core/Linalg.h"
#include "films/BasicFilm.h"
#include <cmath>
#include <memory>
#include <vector>
#include "utilities/ImageIO.h"
#include "debug_helper/DebugFilm.h"

namespace bsdfs
{
    

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
        debug::DebugFilm* film_pdf = new debug::DebugFilm(width,height);
        debug::DebugFilm* film_values = new debug::DebugFilm(width,height);
        debug::DebugFilm* film_combined = new debug::DebugFilm(width,height);
        debug::DebugFilm* film_color = new debug::DebugFilm(width,height);
        debug::DebugFilm* film_dir = new debug::DebugFilm(width, height);
        
        
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
                BXDFResult result = bxdf->EvaluateAll(income_vec, {x_norm,y_norm});
                int x_mapped = result.IncidentDirection[0] * (float) width / 2 + (float)width / 2;
                int y_mapped = result.IncidentDirection[1] * (float) height /2 + (float)height / 2;
                
                core::Vec3 dir = result.IncidentDirection;
                core::Vec3 dir_color = (dir * (core::Prec)0.5) + core::Vec3(0.5, 0.5, 0.5);
                if (result.IncidentDirection[2] < 0)
                {
                    dir_color = { 0,0,1 };
                }
                if (result.IncidentDirection[2] < 0.5)
                    continue;
                film_dir->AddColor(x, y, core::Spectrum::FromRGB(dir_color));
                film_pdf->AddValue(x, y, result.Pdf);
                //std::cout << "Pdf: " << result.Pdf << std::endl;
                film_values->AddValue(x_mapped, y_mapped, 1.f);
                film_color->SetColor(x_mapped, y_mapped, result.Scattering);
                if (result.Scattering.ToRGB()[0] > 0.1)
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
        debug::DebugFilm* film_dist = new debug::DebugFilm(width,height);
        debug::DebugFilm* film_sampled = new debug::DebugFilm(width, height);
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