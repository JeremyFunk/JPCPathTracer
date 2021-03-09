#include "jpc_tracer/maths/maths.h"
#include "jpc_tracer/Concepts.h"

namespace jpctracer {
  
    namespace camera {
        //Ray CameraRay(const CameraSettings& settings, Vec2 pixel,Vec2 random_point);
        template<class T>
        concept CameraFunction = requires (const T camera,Vec2 pixel,Vec2 random_point)
        {
            {camera(pixel,random_point)}->std::convertible_to<Ray>;
        };

    }
    

    template<class T>
    concept CameraBuilder = requires(T builder)
    {
        {Build(builder)}-> camera::CameraFunction;
    };
    

}