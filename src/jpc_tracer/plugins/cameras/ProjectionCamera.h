#pragma once
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/core/maths/Constants.h"
#include "jpc_tracer/engine/PluginsApi.h"
namespace jpctracer::camera
{

    class ProjectionCamera final: public ICamera
    {
    public:
        ProjectionCamera(Prec nearPlane);

        void SetWidthHeight(uint width,uint height);
        Ray Generate(UInt2 pixel,Vec2 random_point) const;

    private:
        Prec _aspect_ratio;
        Prec _near_plane;
        Prec _near_plane_height;
        Prec _near_plane_width;
        Prec _image_width;
        Prec _image_height;
        Prec _image_width_inv;
        Prec _image_height_inv;
    };  




}