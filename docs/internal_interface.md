# Internal Interfaces

## Camerainterface



```cpp
namespace JPCTracer
{
    struct CameraBuilder;

    namespace Camera
    {
        CameraSettings SetupCamera(CameraBuilder builder);

        Ray CameraRay(const CameraSettings& settings, UInt2 pixel,Vec2 random_point);
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
        SamplerState CreateSamplerState(SamplerSettings settings);

        std::vector<Vec2> CreateSamples(SamplerState& state, Int2 Counts);
        std::vector<Vec3> CreateSamples(SamplerState& state, Int3 Counts);
    }

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
        DistributionSettings SetupShader(ShaderBuilder builder);

        struct LightDistributionSettings
        {
            template<DistributionSettings D>
            void AddLight(D* shader,Triangle triangle);
            template<DistributionSettings D>
            void AddLight(D* shader,Point point);
            template<DistributionSettings D>
            void AddLight(D* shader,Sphere sphere);
        };

        enum class MaterialType
        {
            DIFFUSE,
            GLOSSY,
            Transmission,
            
        }

        template<MaterialType type>
        Distribution CreateDistribution(DistributionSettings settings, Ray scattering_ray, SurfaceInteraction interaction);

        std::pair<Spectrum,Prec> EvalDistribution(Distribution condition,Vec3 scattering_dir);
        std::pair<Spectrum,Prec> EvalDistribution(Distribution condition,Vec3& out_incident_dir, Vec2 random_point);
        std::pair<Spectrum,Prec> EvalDistribution(Distribution condition,Vec3& out_incident_dir, Vec3 random_point);
        bool IsDeltaDistribution(const Distribution& condition);

    }

}

```

### Integrator

```cpp

namespace JPCTracer
{
    struct IntegratorSettings;

    namespace Integrator
    {
        enum class RayType
        {
            SHADOW,
            COMBINED,
            GLOSSY_DIRECT,
            GLOSSY_INDIRECT
        };
        struct Payload;

        class Tracer{
            //Forward declaration
            template<RayType type>
            void TraceRay(ray,Payload)
        }

        template<class CameraSettings, class SamplerState>
        Spectrum IntegratePixel(const IntegratorSettings& settings,Int2 pixel, const CameraSettings& camera, SamplerState& sampler,const Tracer& tracer);



        struct AnyHitResult
        {
            bool IsHit;
            bool ShouldTerminate;
        };

        //Lights???


        struct Scene
        {
            IntegratorSettings IntegratorSettings;
            LightDistributionSettings Lights;
        };

        template<RayType type, class Material, class LightsRange>
        AnyHitResult AnyHitProgram(const Scene& scene,const Material& material,
            Ray ray, Payload* payload, SurfaceInteraction interaction, Tracer tracer)
        {
            payload.IsShadow = true;
            return {false,false};
        }

        template<RayType type, class Material>
        void ClosestHitProgram(const  Scene& scene,const Material& material,Ray ray, Payload* payload, SurfaceInteraction interaction,const Tracer& tracer);

        template<RayType type>
        void Miss(const  Scene& scene,Ray ray, Payload* payload);
    }
}
//Example

/*

*/

template<Distribution B,Distribution L>
void DirectLight(const B& bsdf_dist, const L& light_dist, Payload* payload )
{
    Ray light_ray;

    auto[light_val,light_pdf] = EvalDistribution(light_dist,light_ray,payload->Samples2D);
    auto[val_bsdf,pdf_bsdf] = EvalDistribution(bsdf_dist,light_ray);
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
    auto[val,pdf] = EvalDistribution(bsdf_dist,incident_ray,payload->Samples2D);
    auto[val_em,pdf_em] = EvalDistribution(bsdf_light_dist,incident_ray);

    payload->Samples2D++;

    payload->Result += payload->BsdfIntegral * val_em  / pdf_em;
    if(! IsDeltaDistribution(bsdf))
        payload->BsdfIntegral *= val / pdf; 
    else
        payload->BsdfIntegral += val;
    payload->NextRay = incident_ray;
}


template<RayType type, class Bsdf>
void ClosestHitProgram(const Scene& scene,const Material& material,Ray ray, Payload* payload, SurfaceInteraction interaction, Tracer tracer)
{
    auto bsdf_dist = CreateDistribution<Combined>(material,ray,interaction);
    auto bsdf_light_dist = CreateDistribution<Emission>(material,ray,interaction);
    //Direct Light
    for(int i = 0; i< scene.IntegratorSettings.LightSamples; i++)
    {
        auto light_dist = CreateDistribution<Emission>(scene.Lights,ray,interaction);
        DirectLight(bsdt_dist,light_dist);
    }
    for(int i;)
    {
        GlobalIllumination(bsdf_dist,bsdf_light_dist,payload);
        tracer.Trace<Combined>(payload->NextRay,payload);
    }

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
        }
    }

    GeometryRange<Triangle> LoadTriangleMesh(string path);
    GeometryRange<Sphere> LoadSphere(string path);
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
    Vec
} 
```
