#include "SphereSamplers.h"
#include "core/Linalg.h"
#include "core/constants.h"
#include <cmath>
#include "core/constants.h"

namespace samplers
{
    core::Vec3 UniformSampleHemisphere(const core::Vec2& random_point) 
    {
        core::Prec x = std::cos(2*M_PI*random_point[1])*std::sqrt(1-random_point[0]*random_point[0]);
        core::Prec y = std::sin(2*M_PI*random_point[1])*std::sqrt(1-random_point[0]*random_point[0]);
        core::Prec z = random_point[0];
        return {x,y,z};
    }
    
    core::Vec2 ConcentricSampleDisk(const core::Vec2& random_point) 
    {
        core::Prec rand_1 = random_point[0] *2 -1;
        core::Prec rand_2 = random_point[1] *2 -1;

        if(rand_1==0 && rand_2 ==0)
        {
            return {0,0};
        }
        core::Prec radius, theta;
        if(std::abs(rand_1)>std::abs(rand_2))
        {
            radius = rand_1;
            theta = M_PI/4*rand_2/rand_1;
        }
        else{
            radius = rand_2;
            theta = M_PI/2 - M_PI/4*rand_1/rand_2;
        }
        return radius * core::Vec2{std::cos(theta),std::sin(theta)};
    }
    
    core::Vec3 CosinusSampleHemisphere(const core::Vec2& random_point) 
    {
        core::Vec2 d = ConcentricSampleDisk(random_point);
        core::Prec z = std::sqrt(std::max(0.f,1-d[0]*d[0]-d[1]*d[1]));
        return {d[0],d[1],z};
    }
}
