
#include "jpc_tracer/core/MaterialType.h"
#include "jpc_tracer/core/maths/Spectrum.h"
#include "jpc_tracer/plugins/shaders/ShaderContext.h"
#include "jpc_tracer/plugins/shaders/bsdf/BsdfPool.h"
#include "jpc_tracer/plugins/shaders/bsdf/DebugBsdf.h"
#include <cmath>
#include <iostream>

namespace jpctracer {
    inline BsdfNode* DebugBsdf1(ShaderContext context, Spectrum spec)
    {
        return CreateBsdf<MATERIAL_DIFFUSE, DebugBsdfClosure>(context,spec);
    }
    inline BsdfNode* DebugBsdf2(ShaderContext context, Spectrum spec)
    {
        return CreateBsdf<MATERIAL_GLOSSY, DebugBsdfClosure>(context,spec);
    }

    inline BsdfNode* material(ShaderContext context,Prec val)
    {
        Prec val_glossy = std::sqrt(val);
        DebugBsdf1(context, FromRGB({val,val,val}));
        return DebugBsdf2(context, FromRGB({val_glossy,val_glossy,val_glossy}));


    }

}
void glossy_mat(float val,jpctracer::BsdfPool* factory)
{
    jpctracer::NormalSpace normal_space;
    jpctracer::ShaderContext context = {&normal_space, jpctracer::MATERIAL_GLOSSY, factory};
    jpctracer::material(context, val);
     
}

void diffuse_mat(float val,jpctracer::BsdfPool* factory)
{
    
    jpctracer::NormalSpace normal_space;
    jpctracer::ShaderContext context = {&normal_space, jpctracer::MATERIAL_DIFFUSE, factory};
    jpctracer::material(context, val);
     
}
int main(int argc,char *argv[])
{
    std::cout<<"test\n";
    int i;
    jpctracer::BsdfPool factory;
    diffuse_mat((float)argc,&factory);
    return 0;
}
/*

#include<array>
#include<vector>
#include<cmath>
#include<iostream>

enum MatType
{
    BSDF_GLOSSY = 1 << 0,
    BSDF_DIFFUSE = 1<<1,
    BSDF_BSDF = BSDF_GLOSSY|BSDF_DIFFUSE

};

constexpr bool ContainsMatType(int type1,int type2)
{
    return (type1 & type2 ) != 0;
}


struct BsdfFactory
{
    BsdfFactory(MatType t,float* _start) : type(t),result(_start),
    start(_start)
    {
        
    }
    const MatType type;
    float* result;
    float* start;
    template<int type_val,class T>
    void AddValue(T val)
    {
        if(ContainsMatType(type,type_val))
        {
            T* t = (T*) result;
            *t = val;
            t++;
            result = (float*)t;
        }
        
    }
};


struct Context
{
    Context(MatType t, const float* a, const int s,float*result)
        : type(t),arr(a),size(s),factory(t,result)
    {

    }
    const MatType type;
    const float* arr;
    const int size; 
    BsdfFactory factory;
};

float mix(float roughness,float color,float factor)
{
    return roughness*factor + color *(factor-1);
}

std::array<float,4> find_val(const float* arr,int size,int idx)
{
    return {arr[idx],arr[idx+1],arr[idx+2],arr[idx+3]};
}

constexpr std::array<float,4> sqrt_own(float t)
{
    return {t,t,t,t};
}

void uberbsdf(Context context, float roughness, 
                float color)
{
    int a = mix(roughness, 2., roughness);
    float temp = mix(find_val(context.arr,context.size,3)[0],a,a);
    std::array<float,4> b = {temp,temp,temp,temp};
    context.factory.AddValue<BSDF_GLOSSY|BSDF_DIFFUSE>(a);
    context.factory.AddValue<BSDF_DIFFUSE>(b);
    
        
};

float gamma(float val, float gamma)
{
    return (val+gamma)*gamma;
}

void test(float color,float roughness,const float* arr,int size,float* res)
{
    const Context context = {BSDF_GLOSSY,arr,size,res};
    
    auto t = [&](Context c){
        float col2 = gamma(color,size);
        uberbsdf(c,roughness,col2);};
    t(context);
    
}

int main(int argc, char* argv[])
{
    std::array<float,8> t;
    std::array<float,8> res;
    
    test(argc,argc,&t[0],8,&res[0]);
    std::cout<<res[0]<<"\n";
    return res[0];
}*/
