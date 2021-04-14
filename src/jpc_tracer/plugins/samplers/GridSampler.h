#pragma once
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/engine/PluginsApi.h"
#include <random>

namespace jpctracer::sampler {
    namespace detail {

        template<class T>
        concept RandFunc = requires(T func)
        {
            {func()}->std::convertible_to<Prec>;
        };

        template<RandFunc R>
        class GridSampler final: public ISampler
        {
        private:
            R m_rand;
        public:
            GridSampler(R rand) : m_rand{rand} {}

            inline void Sample(UInt2 count,Vec2* result_it)
            {
                Prec inv_width = 1/ (Prec) count[0];
                Prec inv_height = 1/ (Prec) count[1];
                for(int y = 0; y < count[1]; y++)
                {
                    for(int x = 0; x < count[0]; x++)
                    {
                        Prec sample_x = ((Prec) x + m_rand())*inv_width;
                        Prec sample_y = ((Prec) y + m_rand())*inv_height;
                        *result_it = {sample_x,sample_y};
                        result_it++;
                    }
                }
            }

            inline void Sample(UInt3 count,Vec3* result_it)
            {
                Prec inv_width = 1.f/ (Prec) count[0];
                Prec inv_height = 1.f/ (Prec) count[1];
                Prec inv_depth = 1.f / (Prec) count[2];
                for(int z = 0; z < count[2]; z++)
                    for(int y = 0; y < count[1]; y++)
                        for(int x = 0; x < count[0]; x++)
                        {
                            Prec sample_x = ((Prec) x + m_rand())*inv_width;
                            Prec sample_y = ((Prec) y + m_rand())*inv_height;
                            Prec sample_z = ((Prec) z + m_rand())*inv_depth;
                            
                            *result_it = {sample_x,sample_y,sample_z};
                            result_it++;
                        }
            }
            ISampler* Clone() const
            {
                return new GridSampler(m_rand);
            }

        };

        struct RandOp
        {
        private:
            
            std::mt19937 m_gen;
            std::uniform_real_distribution<Prec> dist;
        public:
            RandOp(std::random_device& rd);
            Prec operator()();
        };

    }

    inline auto StratifiedSampler()
    {
        std::random_device rd;
        return detail::GridSampler(detail::RandOp(rd));
    
    };
    inline auto DebugSampler()
    {
        return detail::GridSampler([](){return 0.5;});
    };


    
}