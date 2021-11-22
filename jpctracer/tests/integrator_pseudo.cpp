#include <vector>

typedef float        vec4[4];
typedef float        vec3[3];
typedef float        vec2[2];
typedef unsigned int uint;
typedef float        sampled_color_t[5];
typedef float        hitpoint;

class Integrator
{
  public:
    Integrator(int max_depth);
    void Init();
    bool Next(vec4 direct_color, vec4 indirect_color);
    void GetResult(vec4* result);
};

enum RayState
{
    WORLD,
    LOCAL,
};


class BsdfEvaluator
{
  public:
    BsdfEvaluator(int bsdflimits);
    void LoadBsdf(vec3 incident_dir, hitpoint hit);
    void Eval(vec3* scattered_rays,
              sampled_color_t* out_color,
              vec4*            out_emission);
    void Sample(vec2 rand_p, vec3* out_scattered);
    void CloseBsdf();
};

class Camera
{
  public:
    void GetRay(int x, int y,vec3* out_dir, hitpoint* hit_p);
};

class LightSampler
{
  public:
    LightSampler(int max_light_samples);
    //returns the ligth samples
    int sample(vec3* out_dirs, hitpoint hit, sampled_color_t* out_colors);
};

class Scene
{
  public:
    bool NextHit(hitpoint* hit, vec3 direction);
};

void compute_direct_color(sampled_color_t* bsdf_color, sampled_color_t* light_color, vec4* result);


struct RayMemory
{
    uint max_size;
    uint all_size;
    uint direct_size;
    uint indirect_size;
    vec3* all_dirs;
    vec3* direct_dirs;
    vec3* indirect_dirs;
    sampled_color_t* all_colors;
    sampled_color_t* indirect_colors;
    sampled_color_t* direct_colors;

    void SetDirectSize(uint size);
    void ToLocal(float trans);
    void ToWorld(float trans);
    void Reset();
};


int integrate_tile()
{
    int max_light_samples = 400;
    BsdfEvaluator bsdf_evaluator(100);
    Integrator    integral(5);
    RayMemory rays;
    LightSampler  light_sampler(max_light_samples);
    Camera camera;
    Scene scene;

    for(int y = 0; y<100;y++)
    {
    for(int x = 0; x <100; x++)
    {
        integral.Init();

        
        hitpoint hit_p;

        vec3 next_dir;
        camera.GetRay(x,y,&next_dir,&hit_p);
        
        vec4 direct_color;
        vec4 indirect_color;

        do{
            if(scene.NextHit(&hit_p,next_dir))
            {
                int light_samples = light_sampler.sample(rays.indirect_dirs,hit_p,rays.indirect_colors);
                rays.SetDirectSize(light_samples);
                rays.ToLocal(hit_p);
                bsdf_evaluator.LoadBsdf(next_dir,hit_p);
                vec2 rand = {0.2,0.4};
                bsdf_evaluator.Sample(rand,rays.indirect_dirs);
                vec4 emission;
                bsdf_evaluator.Eval(rays.all_dirs,rays.all_colors, &emission);


            
            }

        }
        while(integral.Next(direct_color,indirect_color));


    }
    }
}
