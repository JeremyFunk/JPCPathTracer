#include "ProjectionCamera.h"



namespace jpctracer
{
    namespace camera::detail
    {
        ProjectionCamera::ProjectionCamera(Prec width, Prec height, Prec nearPlane)
            : _near_plane_width (width / std::max(width, height)),
            _near_plane_height (height / std::max(width, height)),
            _near_plane(nearPlane),
            _image_width(width),
            _image_height(height),
            _image_width_inv(1 / width),
            _image_height_inv(1 / height)
        {
            
        }

        Ray ProjectionCamera::operator()(Vec2 pixel,Vec2 random_point) const{
            
            Vec2 sample = random_point;
            Prec rayX =  (pixel[0] + sample[0]) * _image_width_inv * _near_plane_width * 2 - _near_plane_width;
            Prec rayY = (pixel[1] + sample[1]) * _image_height_inv * _near_plane_height * 2 - _near_plane_height;
            Vec3 dir = Vec3{rayX, -rayY, -_near_plane}.normalized();
            
            Vec3 origin ={0,0,0}; 
            Ray ray{dir, origin};
            ray.LensPosition = pixel + sample;
            return ray;

        }
    }
}