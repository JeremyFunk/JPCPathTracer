#include "ProjectionCamera.h"
#include "jpc_tracer/core/maths/Constants.h"



namespace jpctracer::camera
{
    ProjectionCamera::ProjectionCamera(Prec nearPlane)
        : _near_plane(nearPlane)
        
    {
        SetWidthHeight(300, 200);
    }
    
    void ProjectionCamera::SetWidthHeight(uint width,uint height) 
    {
        Prec widthf = width;
        Prec heightf = height;
        _image_width = widthf;
        _image_height = heightf;
        _image_width_inv = 1.f / widthf;
        _image_height_inv = 1.f / heightf;
        _near_plane_width  = widthf / std::max(widthf, heightf);
        _near_plane_height  = heightf / std::max(widthf, heightf);
    }

    Ray ProjectionCamera::Generate(UInt2 pixel,Vec2 random_point) const{
        
        Prec x = pixel[0];
        Prec y = pixel[1];
        
        Vec2 sample = random_point;
        Prec rayX =  (x + sample[0]) * _image_width_inv * _near_plane_width * 2 - _near_plane_width;
        Prec rayY = (y + sample[1]) * _image_height_inv * _near_plane_height * 2 - _near_plane_height;
        Vec3 dir = Vec3{rayX, -rayY, -_near_plane}.normalized();
        
        Vec3 origin ={0,0,0}; 
        Ray ray{dir, origin};
        ray.LensPosition = Vec2{x,y} + sample;
        return ray;

    }
    
}