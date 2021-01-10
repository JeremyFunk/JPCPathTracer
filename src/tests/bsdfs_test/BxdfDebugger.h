#pragma once
#include "bsdfs/IBXDF.h"
#include "bsdfs/IMicrofacetDistribution.h"
#include "core/Linalg.h"
#include "core/Spectrum.h"
#include <cmath>
#include <memory>
#include <vector>


namespace bsdfs
{
    class DebugFilm
    {
    public:
        DebugFilm(int width, int height);
        void AddValue(int x, int y, float value);
        void AddColor(int x, int y, core::Spectrum spec);
        void SetValue(int x, int y, float value);
        void SetColor(int x, int y, core::Spectrum spec);
        void Save(std::string filepath);
        void SaveLog(std::string filepath);
    private:
        std::vector<float> _pixels;
        int _width;
        int _height;
        float _max_val = 0;

    };
    void BxdfDebugSampleUniform(IBXDF* bxdf, std::string name);
    void BxdfDebugSampleUniform(IBXDF* bxdf, std::string name, core::Vec3 income_vec);
    void DistDebugSampleUniform(IMicrofacetDistribution* dist, std::string name, core::Vec3 income_vec);

}