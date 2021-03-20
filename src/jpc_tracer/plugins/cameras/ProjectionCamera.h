#pragma once
#include "jpc_tracer/core/core.h"
namespace jpctracer
{

    namespace camera {

        class ProjectionCamera
        {
        public:
            ProjectionCamera(Prec width, Prec height, Prec nearPlane);

            Ray operator()(Vec2 pixel,Vec2 random_point) const;

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

    struct ProjectionCameraBuilder
    {
        Prec Width = 300;
        Prec Height = 200; 
        Prec NearPlane = 1;

        auto Build(){
            return camera::ProjectionCamera(Width,Height,NearPlane);
        }
    };


}