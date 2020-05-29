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

        const size_t checkers_size = 16;
    };

    //! A really naive implemenation of a sampler
    template <typename FloatType, typename Int32Type, typename UInt32Type>
    struct naive_sampler2D
    {
        using this_type = naive_sampler2D;

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

        inline friend vec4 texelFetch(const this_type& sampler, const ivec2& p, int32_type lod)
        {
            return sampler.fetch(p);
        }

        // pretend to be a 3D sampler
        inline friend vec4 texture(const this_type& sampler, const vec3& uv)
        {
            return sampler.sample(uv.xy);
        }

        inline friend ivec2 textureSize(const this_type& sampler, int)
        {
            return { sampler.data->width, sampler.data->height };
        }
    private:
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

            // OGL uses left-bottom corner as origin...
            //uv.y = 1.0f - uv.y;

            ivec2 icoord;
            icoord.x = static_cast<int32_type>(uv.x * data->width);
            icoord.y = static_cast<int32_type>(uv.y * data->height);
            icoord.x = max(0, min(static_cast<int>(data->width) - 1, icoord.x));
            icoord.y = max(0, min(static_cast<int>(data->height) - 1, icoord.y));

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

                typename batch_traits<int32_type>::aligned_storage_type istorage;
                auto ibuffer = reinterpret_cast<int32_t*>(&istorage);
                {
                    int32_type index = (data->height - coord.y) * data->pitch_bytes + coord.x * data->bytes_per_pixel;
                    store_aligned(index, ibuffer);
                }

                typename batch_traits<float_type>::aligned_storage_type rstorage;
                typename batch_traits<float_type>::aligned_storage_type gstorage;
                typename batch_traits<float_type>::aligned_storage_type bstorage;
                typename batch_traits<float_type>::aligned_storage_type astorage;

                auto rbuffer = reinterpret_cast<float*>(&rstorage);
                auto gbuffer = reinterpret_cast<float*>(&gstorage);
                auto bbuffer = reinterpret_cast<float*>(&bstorage);
                auto abuffer = reinterpret_cast<float*>(&astorage);

                static_for<0, batch_traits<int32_type>::size>([&](size_t i)
                {
                    auto ptr = data->bytes + ibuffer[i];
                    uint32_t pixel = 0;
                    // TODO: are cache reads aligned?
                    switch (data->bytes_per_pixel)
                    {
                    case 4: pixel |= (ptr[3] << (3 * 8));
                    case 3: pixel |= (ptr[2] << (2 * 8));
                    case 2: pixel |= (ptr[1] << (1 * 8));
                    case 1: pixel |= (ptr[0] << (0 * 8));
                    }

                    // TODO: a faster cast is possible
                    rbuffer[i] = static_cast<float>((pixel & data->rmask) >> data->rshift);
                    gbuffer[i] = static_cast<float>((pixel & data->gmask) >> data->gshift);
                    bbuffer[i] = static_cast<float>((pixel & data->bmask) >> data->bshift);
                    abuffer[i] = static_cast<float>(data->amask ? ((pixel & data->amask) >> data->ashift) : 255);
                });

                vec4 result;
                load_aligned(result.r, rbuffer);
                load_aligned(result.g, gbuffer);
                load_aligned(result.b, bbuffer);
                load_aligned(result.a, abuffer);
                result /= 255.0f;
                return result;
            }
        }
    };
}
