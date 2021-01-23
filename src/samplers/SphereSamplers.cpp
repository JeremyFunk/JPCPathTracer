#include "SphereSamplers.h"
#include "core/Linalg.h"
#include "core/constants.h"
#include <cmath>
#include "core/constants.h"

namespace jpc_tracer
{
    Vec3 UniformSampleHemisphere(const Vec2& random_point) 
    {
        Prec x = std::cos(2*M_PI*random_point[1])*std::sqrt(1-random_point[0]*random_point[0]);
        Prec y = std::sin(2*M_PI*random_point[1])*std::sqrt(1-random_point[0]*random_point[0]);
        Prec z = random_point[0];
        return {x,y,z};
    }
    
    Vec2 ConcentricSampleDisk(const Vec2& random_point) 
    {
        Prec rand_1 = random_point[0] *2 -1;
        Prec rand_2 = random_point[1] *2 -1;

        if(rand_1==0 && rand_2 ==0)
        {
            return {0,0};
        }
        Prec radius, theta;
        if(std::abs(rand_1)>std::abs(rand_2))
        {
            radius = rand_1;
            theta = M_PI/4*rand_2/rand_1;
        }
        else{
            radius = rand_2;
            theta = M_PI/2 - M_PI/4*rand_1/rand_2;
        }
        return radius * Vec2{std::cos(theta),std::sin(theta)};
    }
    
    Vec3 CosinusSampleHemisphere(const Vec2& random_point) 
    {
        Vec2 d = ConcentricSampleDisk(random_point);
        Prec z = std::sqrt(std::max(0.f,1-d[0]*d[0]-d[1]*d[1]));
        return {d[0],d[1],z};
    }
}
