#include "ProjectionCamera.h"
#include "core/ISampler.h"

namespace cameras
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

    Ray ProjectionCamera::GenerateRay(const std::shared_ptr<ISampler>& sampler, Vec2i pixel) const{
        
        Vec2 sample = sampler->Get2DSample();
        Prec rayX =  (pixel[0] + sample[0]) * _image_width_inv * _near_plane_width * 2 - _near_plane_width;
        Prec rayY = (pixel[1] + sample[1]) * _image_height_inv * _near_plane_height * 2 - _near_plane_height;
        Vec3 dir = Vec3(rayX, -rayY, -_near_plane).normalized();
        
        Vec3 origin ={0,0,0}; 
        Ray r(origin, dir);
        Vec2 fpixel = pixel.cast<Prec>();
        r.LensPosition = fpixel + sample;
        return r;

    }
}