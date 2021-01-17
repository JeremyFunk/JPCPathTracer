#include<memory.h>

struct Vec3
{
    float x,y,z;
};
struct Vec2
{
    float x,y;
};

using Spectrum = Vec3;



struct bsdfclosure
{
    virtual Spectrum scattering(void* params, Vec3 scattering_direction,Vec3 incident_direction) = 0;
    virtual float pdf(void* params, Vec3 scattering_direction,Vec3 incident_direction) = 0;
    virtual Vec3 sample(void* params, Vec3 scattering_direction,Vec2 random_point) = 0;   
};


struct bsdfparams;

using bsdfparamsptr = std::unique_ptr<bsdfparams,void(*)(bsdfparams*)>;

bsdfparamsptr create_bsdfparams(int max_size,int max_bsdf_count);

void* malloc_bsdf(bsdfparamsptr root,int size,float weight,bsdfclosure* callback);

Spectrum scattering_bsdf(bsdfparamsptr root, Vec3 scattering_direction,Vec3 incident_direction);

float pdf_bsdf(bsdfparamsptr root, Vec3 scattering_direction,Vec3 incident_direction);

Vec3 sample_bsdf(bsdfparamsptr root, Vec3 scattering_direction,Vec2 random_point);



