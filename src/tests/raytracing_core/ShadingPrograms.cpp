#include "gtest/gtest.h"
#include "jpc_tracer/engine/raytracing/Base.h"
#include "jpc_tracer/engine/raytracing/SceneBuilder.h"
#include "jpc_tracer/engine/raytracing/detail/TracingContext.h"
#include "jpc_tracer/engine/raytracing/detail/Scene.h"

namespace jpctracer::raytracing {
    /*
    struct Payload
    {
        int closest_hits;
        int any_hits;
        int miss_hits;
        int depth;
    };

    struct MaterialSettings
    {
        int max_depth;
    };

    struct MatParameter1
    {
        Vec3 test;
    };


    struct MatParameter2
    {
        Vec3 test1;
        Vec3 test2;
        float debug;
    };

    TracingContext* SetUpShadePrograms(int max_stack_size = 3)
    {
        const Scene* scene = nullptr; 

        TracingContext* context = new TracingContext {ShadePrograms ())};
        context->Programs.SetContext(context);
        return context;
    };



    void CallShadePrograms(TracingContext* context, Payload* payload,const MaterialSettings* setting);

    void ClosestHit(const Scene* scene,const Ray& ray, const SurfaceInteraction& interaction,TracingContext* context,
                                    Payload* input_output_payload,const void* material_settings, void* material_parameters)
    {
        const MaterialSettings* settings = (const MaterialSettings*) material_settings;
        if(settings->max_depth<=input_output_payload->depth)
            return;
        else
            input_output_payload->depth++;
        
        MatParameter2* params = (MatParameter2*) material_parameters;
        Payload payload2;
        payload2 = *input_output_payload;
        payload2.closest_hits+=params->test1[0]*settings->max_depth;
        CallShadePrograms(context,&payload2, settings);
        CallShadePrograms(context,&payload2, settings);
        *input_output_payload = payload2;

    };

    AnyHitResult AnyHit(const Scene* scene,const Ray& ray,const SurfaceInteraction& interaction,
                                    Payload* input_output_payload,const void* material_settings,void* material_parameters)
    {
        input_output_payload->any_hits++;
        return {true,true};
    };

    void Miss(const Scene* scene,const Ray& ray, Payload* payload)
    {
        payload->miss_hits++;
    };

    void MaterialInit(const Scene* scene,const Ray& ray,const SurfaceInteraction& interaction,const void* material_settings, void* material_parameters)
    {
        const MaterialSettings* settings = (const MaterialSettings*) material_settings;
        MatParameter2* params = (MatParameter2*) material_parameters;
        params->debug = 2;
        params->test1 = Vec3{(Prec)settings->max_depth,(Prec)settings->max_depth,(Prec)settings->max_depth};

    };

    void CallShadePrograms(TracingContext* context, Payload* payload,const MaterialSettings* setting)
    {
        
        Ray ray = {Vec3{0,0,0},Vec3{1,1,1},0,0};
        
        const AnyHitFuncPtr anyhit_material_func = &AnyHit;
        const ClosestHitFuncPtr closesthit_material_funcs = &ClosestHit;
        const MaterialInitFuncPtr material_init_params = &MaterialInit;

        const AllocationData params_infos = {sizeof(MatParameter2),alignof(MatParameter2)};

        const void* set = (const void*)setting;

        const void* const * set_ptr =  &set;

        context->Programs.NewRay(ray, payload,&anyhit_material_func , &closesthit_material_funcs, &Miss,  &material_init_params, 
        set_ptr, &params_infos);

        SurfaceInteraction interaction {Vec3(),Vec3(),Vec2(),0,0};

        context->Programs.ExecuteAnyHit(interaction);
        context->Programs.ExecuteClosestHit(interaction);
    }

    TEST(ShadePrograms, test1)
    {
        int max_depth = 3;
        TracingContext* context = SetUpShadePrograms(max_depth+2);
        Payload payload{0,0,0,0};
        MaterialSettings setings{max_depth};
        CallShadePrograms(context, &payload, &setings);

        EXPECT_EQ(payload.any_hits,1+2+4);
        EXPECT_EQ(payload.miss_hits,0);
        EXPECT_EQ(payload.depth,3);
        EXPECT_EQ(payload.closest_hits,max_depth*max_depth*3);
        
        
    }*/
}

