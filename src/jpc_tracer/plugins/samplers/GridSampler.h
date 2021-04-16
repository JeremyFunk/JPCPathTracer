#pragma once
#include "jpc_tracer/core/core.h"
#include "jpc_tracer/engine/PluginsApi.h"
#include <limits>
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
            inline Prec operator()()
            {
                return dist(m_gen); 
            }
        };

        struct RandOpXoroshiro128
        {
            //Reference https://thompsonsed.co.uk/random-number-generators-for-c-performance-tested
            uint64_t shuffle_table[4];
            // The actual algorithm
            inline uint64_t next(void)
            {
                uint64_t s1 = shuffle_table[0];
                uint64_t s0 = shuffle_table[1];
                uint64_t result = s0 + s1;
                shuffle_table[0] = s0;
                s1 ^= s1 << 23;
                shuffle_table[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);
                return result;
            }
            inline Prec operator()()
            {
                return (Prec)next() / (Prec) std::numeric_limits<uint64_t>::max(); 
            }
        };

    }

    inline auto StratifiedSampler()
    {
        std::random_device rd;
        return detail::GridSampler(detail::RandOp(rd));
    
    };
    inline auto StratifiedSampler2()
    {
        return detail::GridSampler(detail::RandOpXoroshiro128());
    
    };
    inline auto DebugSampler()
    {
        return detail::GridSampler([](){return 0.5;});
    };


    
}