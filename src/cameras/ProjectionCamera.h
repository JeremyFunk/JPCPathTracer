#pragma once
#include "core/ICamera.h"

namespace cameras
{
    using namespace core;
    class ProjectionCamera : public ICamera
    {
    public:
        ProjectionCamera(Prec width, Prec height, Prec nearPlane);

        virtual Ray GenerateRay(const std::shared_ptr<ISampler>& sampler, Vec2i pixel) const override ;

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