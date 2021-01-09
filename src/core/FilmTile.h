#pragma once
#include "SpectrumPasses.h"
#include <memory>
#include <vector>
#include "IFilter.h"
#include "core/Bounds2D.h"
#include "core/Linalg.h"
#include "Bounds2D.h"
namespace core {
    class FilmTile
    {
    public:
        FilmTile(std::shared_ptr<IFilter> filter, Bounds2D<int> bounds);
        void AddSample(const SpectrumPasses& spectrum,Vec2 image_position,Prec weight = 1);
        void WritePixels(SpectrumPasses* destiniation);

        //x,y Image Space
        SpectrumPasses GetSpectrumAt(int x,int y) const;

        const Bounds2D<int> Bounds;

    private:
        std::vector<Prec> _weights;
        std::vector<SpectrumPasses> _pixel_unnormalized;

        void add_to_weight(int x,int y, Prec weight);
        void add_to_pixel_unnormalized(int x, int y, SpectrumPasses pixel);

        
        std::shared_ptr<IFilter> _filter;
        int _width = -1;
    };
}