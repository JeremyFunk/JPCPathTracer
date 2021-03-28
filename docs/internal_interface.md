# Internal Interfaces

## Camerainterface

```cpp
namespace jpc_tracer
{
    struct CameraBuilder;

    namespace camera
    {
        CameraFunction Build(CameraBuilder builder);

        Ray CameraFunction(Vec2 pixel,Vec2 random_point);
    }
}
```

 
## Samplerinterface

```cpp
namespace JPCTracer
{
    struct SamplerSettings;

    namespace Sampler
    {
        SamplerFunction Build(SamplerSettings settings);
        template<class IT>
        void SamplerFunction(UInt2 Counts, IT result);
        template<class IT>
        void SamplerFunction(UInt3 Counts, IT result);
    }

}
```


## Animatiom

```cpp

namespace JPCTracer
{

    struct Transformation
    {
        Mat3x3 ScaleRotationMatrix;
        Vec3 Translation;
    }

    Vec3 Apply(Transformation trans,Vec3 v);

    template<class T>
    class IAnimated
    {
        virtual T Get(Prec time) = 0;
        virtual T GetInverse(Prec time) = 0;
        virtual T GetMin(Prec min_time, Prec max_time) = 0;
        virtual T GetMax(Prec min_time, Prec max_time) = 0;
    };


}

```

## Geometry

```cpp

namespace JPCTracer
{

    namespace Geometry
    {
        struct Triangle
        {
            ...
        };

        struct Sphere
        {
            ...
        };

        template<class Shape = Triangle | Sphere>
        class GeometryRange
        {
        public:
            Shape* begin();
            Shape* end();
            
            std::shared_prt<const IAnimated<Transformation>> Animation;
        }
    }

    GeometryRange<Triangle> LoadTriangleMesh(string path);
    GeometryRange<Sphere> LoadSphere(string path);

}

```


## Appearance

### Shader


```cpp
namespace JPCTracer
{
    struct ShaderBuilder;


    namespace Shader
    {
        enum class ObjectType
        {
            Triangle;
            Sphere;
        }

        BsdfNode* Shader(ShaderContext context);

        class DistributionFactory
        {
            DistributionFactory Clone();
            template<MaterialType type>
            RootShader CreateShader(Shader shader, Ray scattering_ray, SurfaceInteraction interaction);
            
            template<MaterialType type>
            LightsDistribution CreateLights(const Lights& lights, Ray scattering_ray, SurfaceInteraction interaction);
            
        }

        struct Lights
        {
            template<class MaterialBuilder>
            void AddPointLight(Shader shader,Vec3 pos);
            void AddSunLight(Shader shader,Vec3 pos, Vec3 dir);
            
        };

        enum class MaterialType
        {
            BSDF,
            DIFFUSE,
            GLOSSY,
            Transmission,
            SUBSURFACE,
            EMISSION,
            TRANSPARENCY,
        }

        //DistributionSettings = Lights | Shader
        //template<MaterialType type>
        //DistributionFunction CreateDistribution(ShaderThreadData,DistributionSettings settings, Ray scattering_ray, SurfaceInteraction interaction);
        //returns value and the pdf
        //if pdf = 0 then the Distirbution is a Delta Distirubtion
        std::pair<Spectrum,Prec> DistributionFunction(const Ray& incident_ray) const;
        std::pair<Spectrum,Prec> DistributionFunction(Ray& out_incident_ray, Vec2 random_point) const;
        std::pair<Spectrum,Prec> DistributionFunction(Ray& out_incident_ray, Vec3 random_point) const;
        bool IsDeltaDistribution(Prec pdf);

    }

}

```
### Film

```cpp
    template<class T>
    concept PixelSaver = requires(T saver, std::string channel_name,
                                        uint x, uint y, Vec3 rgb)
    {
        saver(channel_name,x,y,rgb);
    };
```

### Integrator

```cpp

namespace JPCTracer
{
    
    struct IntegratorSettings;

    namespace Integrator
    {
        struct Payload;

        struct TraceRay{
            //Forward declaration
            template<MaterialType type>
            void operator()(Ray ray,Payload* payload);
        }
                
        struct RayBehavior
        {
            //returnes if it was an hit
            template<class BsdfDistribution>
            bool AnyHitProgram(const BsdfDistribution& material,Payload* payload);

            template<class BsdfDistribution>
            void ClosestHitProgram(const LightsDistribution& lights,const BsdfDistribution& bsdf,const Distribution& material_emission,Payload* payload ,TraceRay& trace);

            template<class BackbroundDistribution>
            void Miss(const BackbroundDistribution& background, Payload* payload);
        }

        template<class CameraSettings, class Sampler, class PixelSaver>
        void Integrator(Int2 pixel, const CameraSettings& camera, Sampler& sampler,TraceRay& trace, PixelSaver& saver);
        
        template<MaterialType type>
        RayBehavior GetRayBehavior(Integrator i)
    }

    Integrator Build(IntegratorSettings)

    

    
}
//Example

/*

*/

template<Distribution B,Distribution L>
void DirectLight(const B& bsdf_dist, const L& light_dist, Payload* payload )
{
    Ray light_ray;

    auto[light_val,light_pdf] = light_dist(light_ray,payload->Samples2D);
    auto[val_bsdf,pdf_bsdf] = bsdf_dist(light_ray);
    payload->Samples2D++;

    
    if(! IsDeltaDistribution(bsdf))
        payload->Result += MultipleImportance(light_val, light_pdf, val_bsdf,pdf_bsdf); 
    else
        payload->BsdfIntegral += light_val * val_bsdf;
}


template<Distribution B,Distribution L>
void GlobalIllumination(const B& bsdf_dist, const L& bsdf_light_dist, Payload* payload)
{
    //Bsdf
    Ray incident_ray;
    using MaterialType = type::Material;
    auto[val,pdf] = bsdf_dist(incident_ray,payload->Samples2D);
    auto[val_em,pdf_em] = bsdf_light_dist(incident_ray);

    payload->Samples2D++;

    payload->Result += payload->BsdfIntegral * val_em  / pdf_em;
    if(! IsDeltaDistribution(bsdf))
        payload->BsdfIntegral *= val / pdf; 
    else
        payload->BsdfIntegral *= val;
    
    payload->NextRay = incident_ray;
}

...

template<RayType type>
struct GenerallRays
{  
    int MaxDepth;
    int LightSampleCount;
    int GlobalIlluminationCount;

    template<class Bsdf>
    void ClosestHitProgram(const LightsDistirbution& lights,const Bsdf& bsdf,const Distribution& emission,Payload* payload ,TraceRay& trace)
    {
        if(payload.Depth>=MaxDepth) return;

        //Direct Light
        for(int i = 0; i< LightSampleCount; i++)
        {
            
            DirectLight(bsdf,lights);
        }
        for(int i = 0; i < GlobalIlluminationCount;i++)
        {
            GlobalIllumination(bsdf,emission,payload);
            trace<Combined>(payload->NextRay,payload);
        }
    }

    template<class Material, class LightsRange>
    bool AnyHitProgram(const MaterialDistribution& material,Payload* payload)
    {
        return true;
    };
    void Miss(const BackbroundDistribution& background, Payload* payload)
    {
        Vec3 incident_ray;
        auto[val,pdf] = background(incident_ray,Vec2());
        payload.result += payload->BSDFIntegral* val;
    }


}

```


## Engine

```cpp
namespace JPCTracer
{
    template<class IntegratorSettings>
    struct Engine
    {
        template<class IntegratorSettings2>
        Engine<IntegratorSettings2> SetIntegrator(IntegratorSettings2 settings);

        template<class GeometryRange, class MaterialBuilder>
        void AddObject(GeometryRange g, MaterialBuilder b);

        template<class GeometryRange, class MaterialBuilder, class EmmissionBuilder>
        void AddObject(GeometryRange g, MaterialBuilder m, EmmissionBuilder e);
        
        template<class GeometryRange, class EmmissionBuilder>
        void AddLight(GeometryRange g, EmmissionBuilder e);

        
    }
}


```

## Math

```cpp
namespace JPCTracer
{
    Vec<T,_Size>
    Mat<T,_Rows,_Cols>;
} 
```
