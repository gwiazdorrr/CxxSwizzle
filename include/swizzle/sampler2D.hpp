// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <cstdint>
#include <vector>
#include <swizzle/detail/utils.h>
#include <swizzle/vector.hpp>
#include <type_traits>

namespace swizzle
{
    struct naive_sampler_data
    {
        enum wrap_modes
        {
            clamp,
            repeat,
            mirror_repeat
        };

        wrap_modes wrap_mode = repeat;

        unsigned width = 256;
        unsigned height = 256; 
        
        uint32_t rmask = 0;
        uint32_t gmask = 0;
        uint32_t bmask = 0;
        uint32_t amask = 0;

        uint8_t rshift = 0;
        uint8_t gshift = 0;
        uint8_t bshift = 0;
        uint8_t ashift = 0;

        uint8_t* bytes = nullptr;
        unsigned bytes_per_pixel = 0;
        unsigned pitch_bytes = 0;

        bool is_floating_point = false;
    };

    //! A really naive implemenation of a sampler
    template <typename FloatType, typename Int32Type, typename UInt32Type>
    struct naive_sampler_generic
    {
        using this_type = naive_sampler_generic;

        using float_type = FloatType;
        using uint32_type = UInt32Type;
        using int32_type = Int32Type;

        using vec2 = vector<float_type, 2>;
        using vec3 = vector<float_type, 3>;
        using vec4 = vector<float_type, 4>;
        using ivec2 = vector<int32_type, 2>;

        static inline const vec4 checkers0 = vec4(0.6f);
        static inline const vec4 checkers1 = vec4(0.9f);
        static inline const naive_sampler_data empty_data = {};

        const naive_sampler_data* data = &empty_data;
        size_t face_count = 1;


        inline friend vec4 textureLod(const this_type& sampler, const vec2& uv, const float_type& lod)
        {
            return sampler.sample(uv);
        }

        inline friend vec4 texture(const this_type& sampler, const vec2& uv, const float_type& bias)
        {
            return sampler.sample(uv);
        }

        inline friend vec4 texture(const this_type& sampler, const vec2& uv)
        {
            return sampler.sample(uv);
        }

        inline friend vec4 textureGrad(const this_type& sampler, const vec2& uv, const vec2& dPdx, const vec2& dPdy)
        {
            return sampler.sample(uv);
        }

        inline friend vec4 texelFetch(const this_type& sampler, const ivec2& p, int32_type lod)
        {
            return sampler.fetch(p);
        }

        inline friend vec4 texture(const this_type& sampler, const vec3& uv)
        {
            return sampler.sample(uv);
        }

        inline friend ivec2 textureSize(const this_type& sampler, int)
        {
            return { sampler.data->width, sampler.data->height };
        }
    private:

        template <typename IntType, typename FloatType>
        std::tuple<IntType, IntType> uv_to_icoord(FloatType u, FloatType v, const naive_sampler_data* data) const {
            using std::min;
            auto ix = static_cast<IntType>(u * data->width);
            auto iy = static_cast<IntType>(v * data->height);
            ix = min(IntType((int)data->width - 1), ix);
            iy = min(IntType((int)data->height - 1), iy);
            return std::make_tuple(ix, iy);
        }

        template <typename IntType>
        IntType icoord_to_index(IntType x, IntType y, const naive_sampler_data* data) const
        {
            return (data->height - 1 - y) * data->pitch_bytes + x * data->bytes_per_pixel;
        }

        std::tuple<float, float, float, float> fetch_pixel(int index, const naive_sampler_data* data) const
        {
            auto ptr = data->bytes + index;

            if (data->is_floating_point) {
                return std::make_tuple(
                    reinterpret_cast<const float*>(ptr)[0],
                    reinterpret_cast<const float*>(ptr)[1],
                    reinterpret_cast<const float*>(ptr)[2],
                    reinterpret_cast<const float*>(ptr)[3]
                );
            }
            else
            {
                uint32_t pixel = 0;
                // TODO: are cache reads aligned?
                // TODO: still crashing
                switch (data->bytes_per_pixel)
                {
                case 4: pixel |= (ptr[3] << (3 * 8));
                case 3: pixel |= (ptr[2] << (2 * 8));
                case 2: pixel |= (ptr[1] << (1 * 8));
                case 1: pixel |= (ptr[0] << (0 * 8));
                }

                // TODO: a faster cast is possible
                return std::make_tuple(
                    static_cast<float>((pixel & data->rmask) >> data->rshift) / 255.0f,
                    static_cast<float>((pixel & data->gmask) >> data->gshift) / 255.0f,
                    static_cast<float>((pixel & data->bmask) >> data->bshift) / 255.0f,
                    static_cast<float>(data->amask ? ((pixel & data->amask) >> data->ashift) / 255.0f : 1.0f)
                );
            }
        }

        vec4 sample(const vec3& _coord) const
        {
            // this shit is going to be problematic...
            vec3 coord = coord.call_normalize(_coord);

            using namespace swizzle::detail;

            alignas(batch_traits<float_type>::align) float px[batch_traits<float_type>::size];
            alignas(batch_traits<float_type>::align) float py[batch_traits<float_type>::size];
            alignas(batch_traits<float_type>::align) float pz[batch_traits<float_type>::size];
            alignas(batch_traits<float_type>::align) float pr[batch_traits<float_type>::size];
            alignas(batch_traits<float_type>::align) float pg[batch_traits<float_type>::size];
            alignas(batch_traits<float_type>::align) float pb[batch_traits<float_type>::size];
            alignas(batch_traits<float_type>::align) float pa[batch_traits<float_type>::size];

            store_aligned(coord.x, px);
            store_aligned(coord.y, py);
            store_aligned(coord.z, pz);

            static_for<0, batch_traits<float_type>::size>([&](size_t i)
            {
                float x = px[i];
                float y = py[i];
                float z = pz[i];
                
                float ax = std::abs(x);
                float ay = std::abs(y);
                float az = std::abs(z);

                int face_index;
                float u;
                float v;

                if (ax >= ay && ax >= az ) 
                {
                    float s = x > 0 ? 1 : -1;
                    face_index = 0 + (s > 0 ? 0 : 1);
                    u = -1 * z / x;
                    v =  s * y / x;
                }
                else if (ay >= az && ay >= az) 
                {
                    float s = y > 0 ? 1 : -1;
                    face_index = 2 + (s > 0 ? 0 : 1);
                    u =  s * x / y;
                    v = -s * z / y;
                }
                else
                {
                    float s = z > 0 ? 1 : -1;
                    face_index = 4 + (s > 0 ? 0 : 1);
                    u = 1 * x / z;
                    v = s * y / z;
                }

                u *= 0.5f;
                v *= 0.5f;
                u += 0.5f;
                v += 0.5f;

                auto face_data = data + face_index;
                int ix, iy;
                std::tie(ix, iy) = uv_to_icoord<int>(u, v, face_data);
                std::tie(pr[i], pg[i], pb[i], pa[i]) = fetch_pixel(icoord_to_index(ix, iy, face_data), face_data);
            });

            vec4 result;
            load_aligned(result.r, pr);
            load_aligned(result.g, pg);
            load_aligned(result.b, pb);
            load_aligned(result.a, pa);
            return result;
        }

        vec4 sample(const vec2& coord) const
        {
            vec2 uv;

            switch (data->wrap_mode)
            {
            case naive_sampler_data::repeat:
                uv = vec2::call_mod(coord, 1.0f);
                break;
            case naive_sampler_data::mirror_repeat:
                uv = vec2::call_abs(vec2::call_mod(coord - 1.0f, 2.0f) - 1.0f);
                break;
            default:
                uv = vec2::call_clamp(coord, 0.0f, 1.0f);
                break;
            }

            ivec2 icoord;
            std::tie(icoord.x, icoord.y) = uv_to_icoord<int32_type>(uv.x, uv.y, data);
            return fetch(icoord);
        }




        vec4 fetch(const ivec2& coord) const
        {
            if (!data->bytes)
            {
                return vec4(0.0f, 0.0f, 0.0f, 1.0f);
            }
            else
            {
                using namespace swizzle::detail;

                alignas(batch_traits<int32_type>::align) int ibuffer[batch_traits<int32_type>::size];
                {
                    int32_type index = icoord_to_index(coord.x, coord.y, data);
                    store_aligned(index, ibuffer);
                }

                alignas(batch_traits<float_type>::align) float pr[batch_traits<float_type>::size];
                alignas(batch_traits<float_type>::align) float pg[batch_traits<float_type>::size];
                alignas(batch_traits<float_type>::align) float pb[batch_traits<float_type>::size];
                alignas(batch_traits<float_type>::align) float pa[batch_traits<float_type>::size];

                static_for<0, batch_traits<int32_type>::size>([&](size_t i)
                {
                    std::tie(pr[i], pg[i], pb[i], pa[i]) = fetch_pixel(ibuffer[i], data);
                });

                vec4 result;
                load_aligned(result.r, pr);
                load_aligned(result.g, pg);
                load_aligned(result.b, pb);
                load_aligned(result.a, pa);
                return result;
            }
        }
    };
}
