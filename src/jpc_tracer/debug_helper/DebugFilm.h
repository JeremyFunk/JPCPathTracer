#pragma once
#include "core/Spectrum.h"


namespace jpc_tracer {
    class DebugFilm
    {
    public:
        DebugFilm(int width, int height);
        void AddValue(int x, int y, float value);
        void AddColor(int x, int y, Spectrum spec);
        void SetValue(int x, int y, float value);
        void SetColor(int x, int y, Spectrum spec);
        void Save(std::string filepath);
        void SaveLog(std::string filepath);
    private:
        std::vector<float> _pixels;
        int _width;
        int _height;
        float _max_val = 0;

    };
}