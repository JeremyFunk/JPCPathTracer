#pragma once
#include "jpc_tracer/core/Logger.h"
#include "jpc_tracer/core/MaterialType.h"
#include"jpc_tracer/core/core.h"
#include "NormalSpace.h"
#include "jpc_tracer/engine/shadersystem/IBsdfClosure.h"
#include <iostream>

namespace jpctracer::shadersys
{

    struct ShaderResults
    {
        ShaderResults(unsigned short eval_size,unsigned short sampling_size)
        {
            assert(eval_size+sampling_size<75);
            assert(sampling_size<50);
            
            sampled_rays = {&rays[0],sampling_size};
            
            eval_results = {&results[0],eval_size};
            sampled_results = {&results[0]+eval_size,sampling_size};
        }

        ShaderResults(const ShaderResults& shres)
            : rays(shres.rays),results(shres.results)
        {
            unsigned short eval_size = shres.eval_results.size;
            unsigned short sampling_size = shres.sampled_results.size;
            sampled_rays = {&rays[0],sampling_size};            
            eval_results = {&results[0],eval_size};
            sampled_results = {&results[0]+eval_size,sampling_size};
        }

        ShaderResults(ShaderResults&& shres)
            : rays(std::move(shres.rays)),results(std::move(shres.results))
        {
            unsigned short eval_size = shres.eval_results.size;
            unsigned short sampling_size = shres.sampled_results.size;
            sampled_rays = {&rays[0],sampling_size};            
            eval_results = {&results[0],eval_size};
            sampled_results = {&results[0]+eval_size,sampling_size};
        }
        View<Ray> sampled_rays;
        View<ShaderResult> sampled_results;
        View<ShaderResult> eval_results;
        Spectrum emission = Black();
    private:
        std::array<Ray,50> rays;
        std::array<ShaderResult,75> results; 
        

    };

    #define JPCMAXNODES 50

    struct BsdfNode
    {
        int id =-1;
        bool is_leaf = false;
        Spectrum emission = Black();
    };

    enum class BsdfState
    {
        WEIGHTS = 0,
        SAMPLING = 1,
        EVALUATION = 2
    };


    struct BsdfLinks
    {
        struct Node {
            int id = -1;
            bool is_leaf = false;
            Node(const BsdfNode& n): id(n.id),is_leaf(n.is_leaf) {}
            Node() {};
        };
        Node left;
        Node right;
        Prec left_weight = 1;
        Prec right_weight = 1;
    };

    struct BsdfMemory
    {
        
        View<Ray> input_rays;
        View<Vec2> samples2D;
        ShaderResults results;
        std::array<Prec,JPCMAXNODES> weights = {};
        int bsdf_idx = 0;
        int bsdf_count = -1;
        int bsdfnodes_count = 0;

       
        std::array<BsdfLinks,JPCMAXNODES> links;
        std::array<std::pair<int,int>,JPCMAXNODES> sampled_ranges;

        
    };

    struct ShaderContext
    {
        
        const MaterialType ray_type;
        BsdfState state;
        BsdfMemory* memory;
        //in normal space
        const NormalSpace& normal_space;
    };


    template<std::derived_from<IBsdfClosure> BsdfT>
    constexpr void SampleBsdf(ShaderContext& ctx,const BsdfT& bsdf,int bsdf_id)
    {
        BsdfMemory* mem = ctx.memory;
        auto[left,right] = mem->sampled_ranges[bsdf_id];
        Prec weight = mem->weights[bsdf_id];
        for(int i = left;i<right;i++)
        {
            Ray& out_ray = mem->results.sampled_rays[i];
            const Vec2& in_rand = mem->samples2D[i];
            //TypeDisplay<BsdfT> bsdf_type;
            mem->results.sampled_results[i]+=bsdf.Sample2D(&out_ray,in_rand)*weight;
        }
    }

    template<std::derived_from<IBsdfClosure> BsdfT>
    constexpr void EvalBsdf(ShaderContext& ctx,const BsdfT& bsdf,int bsdf_id)
    {
        BsdfMemory* mem = ctx.memory;
        auto[left,right] = mem->sampled_ranges[bsdf_id];
        Prec weight = mem->weights[bsdf_id];
        for(int i = 0;i<mem->samples2D.size;i++)
        {
            //skip already computed rays
            if(i==left) i=right;
            const Ray& out_ray = mem->results.sampled_rays[i];
            mem->results.sampled_results[i]+=bsdf.Eval(out_ray)*weight;
        }
        for(int i=0;i<mem->input_rays.size;i++)
        {
            const Ray& out_ray = mem->input_rays[i];
            mem->results.eval_results[i]+=bsdf.Eval(out_ray)*weight;
        }
    }    

    template<std::derived_from<IBsdfClosure> BsdfT,class... Args>
    constexpr BsdfNode __CreateBsdf(ShaderContext& ctx, Args&&... args)
    {
        int bsdf_id = ctx.memory->bsdf_idx;
        ctx.memory->bsdf_idx++;

        switch(ctx.state)
        {                
            case BsdfState::WEIGHTS:
                break;
                
            case BsdfState::SAMPLING: 
                {
                    BsdfT bsdf(std::forward<Args>(args)...);
                    SampleBsdf(ctx,bsdf,bsdf_id);
                }
                break;
            case BsdfState::EVALUATION: 
                {
                    BsdfT bsdf(std::forward<Args>(args)...);
                    EvalBsdf(ctx,bsdf,bsdf_id);
                }
                break;
            
        }
        return {bsdf_id,true};
    }


    inline BsdfNode __MixBsdf(ShaderContext& ctx, BsdfNode node1,BsdfNode node2, Prec weight)
    {
        if(ctx.state==BsdfState::WEIGHTS)
        {
            BsdfMemory* mem = ctx.memory;
            if(node1.id==-1) return node2;
            if(node2.id==-1) return node1;

            uint bsdfnode_id = mem->bsdfnodes_count;
            mem->links[bsdfnode_id] = BsdfLinks{node1,node2,weight,1-weight};
            mem->bsdfnodes_count++;
            return {bsdfnode_id,false,node1.emission*weight+node2.emission*(1-weight)};
        }
        return {-1,true};
    }

    inline void ComputeWeights(BsdfNode root_node,BsdfMemory& bsdf_mem)
    {
        if(root_node.is_leaf) 
        {
            if(bsdf_mem.weights[root_node.id]==0)
                bsdf_mem.weights[root_node.id] = 1;
            else
                bsdf_mem.weights[root_node.id]*=1;
        }
        else
        {
            for(int bsdf_idx = root_node.id; bsdf_idx>=0;bsdf_idx--)
            {
                auto& links = bsdf_mem.links;
                auto&[left,right,left_weight,right_weight] = links[bsdf_idx];
                if(! left.is_leaf) 
                {
                    links[left.id].left_weight*= left_weight;
                    links[left.id].right_weight *= left_weight;

                }
                else 
                {
                    if(bsdf_mem.weights[left.id]==0)
                        bsdf_mem.weights[left.id]= left_weight;
                    else
                        bsdf_mem.weights[left.id] *= left_weight;
                }
                if (!right.is_leaf)
                {
                    links[right.id].left_weight *= right_weight;
                    links[right.id].right_weight *= right_weight;
                }
                else
                {
                    if (bsdf_mem.weights[right.id] == 0)
                        bsdf_mem.weights[right.id] = right_weight;
                    else
                        bsdf_mem.weights[right.id]*= right_weight;
                }
                assert(left.is_leaf || left.id<bsdf_idx);
                assert(right.is_leaf || right.id<bsdf_idx);
                
            }
        }
        
    }

    inline std::array<std::pair<int,int>,JPCMAXNODES> SetSamplingRanges(const std::array<Prec,JPCMAXNODES>& weights, View<Vec2> samples,int bsdf_count)
    {
        std::array<std::pair<int,int>,JPCMAXNODES> sampling_ranges;
        std::sort(samples.begin(),samples.end(),[](const Vec2& v1,const Vec2& v2)
        {
            return v1[0]<v2[0];
        });
        Prec last_sum = 0;
        Prec next_sum = 0;
        
        int samples_idx = 0;
        for(int bsdf_idx = 0;bsdf_idx<bsdf_count;bsdf_idx++)
        {
            next_sum+=weights[bsdf_idx];
            int last_samples_idx = samples_idx;
            while(next_sum>=samples[samples_idx][0] && samples_idx<samples.size)
            {
                samples[samples_idx][0]-=last_sum;
                samples[samples_idx][0]/=next_sum-last_sum;
                samples_idx++;
            }
            sampling_ranges[last_samples_idx].first = last_samples_idx;
            sampling_ranges[last_samples_idx].second = samples_idx;
            last_sum = next_sum;
        }
        return sampling_ranges;

    }

    template<MaterialType type,class ShaderFuncT>
    ShaderResults SampleShader(ShaderFuncT shader_func,const NormalSpace& normal_space,View<Ray> rays, View<Vec2> samples)
    {
        BsdfMemory bsdf_mem = {rays,samples,ShaderResults{(unsigned short)rays.size,(unsigned short)samples.size}};
        //bsdf_mem.weights.fill(1);

        for(int i=0;i<3;i+=1)
        {
            ShaderContext context{type,(BsdfState) i,&bsdf_mem,normal_space};
            BsdfNode root_node = shader_func(context);
            if(i==0)
            {
                bsdf_mem.results.emission = root_node.emission;
                if(bsdf_mem.bsdf_idx==0) return bsdf_mem.results; 
                bsdf_mem.bsdf_count=bsdf_mem.bsdf_idx;

                ComputeWeights(root_node,bsdf_mem);
                bsdf_mem.sampled_ranges = SetSamplingRanges(bsdf_mem.weights,bsdf_mem.samples2D,bsdf_mem.bsdf_count);
            }
            bsdf_mem.bsdf_idx = 0;
        }
        return bsdf_mem.results;
    }

    template<MaterialType type,class ShaderFuncT>
    ShaderResults EvalShader(ShaderFuncT shader_func,const NormalSpace& normal_space,View<Ray> rays)
    {
        
        BsdfMemory bsdf_mem = {rays,{nullptr,0},ShaderResults{(unsigned short)rays.size,0}};
        //bsdf_mem.weights.fill(1);

        
        
        for(int i=0;i<3;i+=2)
        {
            bsdf_mem.bsdf_idx = 0;
            ShaderContext context{type,(BsdfState) i,&bsdf_mem,normal_space};
            BsdfNode root_node = shader_func(context);
            if(i==0)
            {
                bsdf_mem.results.emission = root_node.emission;
                if(bsdf_mem.bsdf_idx==0) return bsdf_mem.results;
                bsdf_mem.bsdf_count=bsdf_mem.bsdf_idx;

                ComputeWeights(root_node,bsdf_mem);
            }
            bsdf_mem.bsdf_idx = 0;   
        }
        return bsdf_mem.results;

    }

    //All rays should be in the Normal space
    struct IShader
    {
        virtual ShaderResults EvalDIFFUSE        (const NormalSpace& normal_space, View<Ray> rays) const = 0;
        virtual ShaderResults EvalGLOSSY         (const NormalSpace& normal_space, View<Ray> rays) const = 0;
        virtual ShaderResults EvalTRANSMISSION   (const NormalSpace& normal_space, View<Ray> rays) const = 0;
        virtual ShaderResults EvalSUBSURFACE     (const NormalSpace& normal_space, View<Ray> rays) const = 0;
        virtual ShaderResults EvalEMISSION       (const NormalSpace& normal_space, View<Ray> rays) const = 0;
        virtual ShaderResults EvalTRANSPARENCY   (const NormalSpace& normal_space, View<Ray> rays) const = 0;
        virtual ShaderResults EvalBSDF           (const NormalSpace& normal_space, View<Ray> rays) const = 0;
        
        virtual ShaderResults SampleDIFFUSE        (const NormalSpace& normal_space, View<Ray> rays, View<Vec2> samples) const = 0;
        virtual ShaderResults SampleGLOSSY         (const NormalSpace& normal_space, View<Ray> rays, View<Vec2> samples) const = 0;
        virtual ShaderResults SampleTRANSMISSION   (const NormalSpace& normal_space, View<Ray> rays, View<Vec2> samples) const = 0;
        virtual ShaderResults SampleSUBSURFACE     (const NormalSpace& normal_space, View<Ray> rays, View<Vec2> samples) const = 0;
        virtual ShaderResults SampleEMISSION       (const NormalSpace& normal_space, View<Ray> rays, View<Vec2> samples) const = 0;
        virtual ShaderResults SampleTRANSPARENCY   (const NormalSpace& normal_space, View<Ray> rays, View<Vec2> samples) const = 0;
        virtual ShaderResults SampleBSDF           (const NormalSpace& normal_space, View<Ray> rays, View<Vec2> samples) const = 0;

        virtual ~IShader() {}
    };

    template<class ShaderFuncT>
    struct Shader final: public IShader
    {
        Shader(const ShaderFuncT& _func) : func(_func) {}
        ShaderFuncT func;

        ShaderResults EvalDIFFUSE        (const NormalSpace& n_space, View<Ray> rays) const {return EvalShader<MATERIAL_DIFFUSE>      (func,n_space, rays);}
        ShaderResults EvalGLOSSY         (const NormalSpace& n_space, View<Ray> rays) const {return EvalShader<MATERIAL_GLOSSY>       (func,n_space, rays);}
        ShaderResults EvalTRANSMISSION   (const NormalSpace& n_space, View<Ray> rays) const {return EvalShader<MATERIAL_TRANSMISSION> (func,n_space, rays);}
        ShaderResults EvalSUBSURFACE     (const NormalSpace& n_space, View<Ray> rays) const {return EvalShader<MATERIAL_SUBSURFACE>   (func,n_space, rays);}
        ShaderResults EvalEMISSION       (const NormalSpace& n_space, View<Ray> rays) const {return EvalShader<MATERIAL_EMISSION>     (func,n_space, rays);}
        ShaderResults EvalTRANSPARENCY   (const NormalSpace& n_space, View<Ray> rays) const {return EvalShader<MATERIAL_TRANSPARENCY> (func,n_space, rays);}
        ShaderResults EvalBSDF           (const NormalSpace& n_space, View<Ray> rays) const {return EvalShader<MATERIAL_BSDF>         (func,n_space, rays);}
        
        ShaderResults SampleDIFFUSE        (const NormalSpace& n_space, View<Ray> rays, View<Vec2> samples) const {return SampleShader<MATERIAL_DIFFUSE>      (func,n_space, rays,samples);};
        ShaderResults SampleGLOSSY         (const NormalSpace& n_space, View<Ray> rays, View<Vec2> samples) const {return SampleShader<MATERIAL_GLOSSY>       (func,n_space, rays,samples);};
        ShaderResults SampleTRANSMISSION   (const NormalSpace& n_space, View<Ray> rays, View<Vec2> samples) const {return SampleShader<MATERIAL_TRANSMISSION> (func,n_space, rays,samples);};
        ShaderResults SampleSUBSURFACE     (const NormalSpace& n_space, View<Ray> rays, View<Vec2> samples) const {return SampleShader<MATERIAL_SUBSURFACE>   (func,n_space, rays,samples);};
        ShaderResults SampleEMISSION       (const NormalSpace& n_space, View<Ray> rays, View<Vec2> samples) const {return SampleShader<MATERIAL_EMISSION>     (func,n_space, rays,samples);};
        ShaderResults SampleTRANSPARENCY   (const NormalSpace& n_space, View<Ray> rays, View<Vec2> samples) const {return SampleShader<MATERIAL_TRANSPARENCY> (func,n_space, rays,samples);};
        ShaderResults SampleBSDF           (const NormalSpace& n_space, View<Ray> rays, View<Vec2> samples) const {return SampleShader<MATERIAL_BSDF>         (func,n_space, rays,samples);};
    };


}