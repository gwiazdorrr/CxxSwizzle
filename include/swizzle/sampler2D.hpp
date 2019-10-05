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

        enum wrap_modes
        {
            clamp,
            repeat,
            mirror_repeat
        };

        wrap_modes wrap_mode = repeat;

        unsigned width = 256;
        unsigned height = 256;
        vec3 resolution;

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
        unsigned pitch = 0;

        const size_t checkers_size = 16;

        vec4 checkers0 = vec4(0.6f);
        vec4 checkers1 = vec4(0.9f);

        naive_sampler2D()
        {
            resolution = vec3(static_cast<float>(width), static_cast<float>(height), 0.0f);
        }

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

    private:
        vec4 sample(const vec2& coord) const
        {
            vec2 uv;

            switch (wrap_mode)
            {
            case repeat:
                uv = vec2::call_mod(coord, 1.0f);
                break;
            case mirror_repeat:
                uv = vec2::call_abs(vec2::call_mod(coord - 1.0f, 2.0f) - 1.0f);
                break;
            case clamp:
            default:
                uv = vec2::call_clamp(coord, 0.0f, 1.0f);
                break;
            }

            // OGL uses left-bottom corner as origin...
            uv.y = 1.0f - uv.y;

            ivec2 icoord;
            icoord.x = min(width - 1, int32_type(uv.x * width));
            icoord.y = min(height - 1, int32_type(uv.y * height));
            return fetch(icoord);
        }

        vec4 fetch(const ivec2& coord) const
        {
            if (!bytes)
            {
                auto remainders = vec2::call_mod(vec2(coord), static_cast<float>(checkers_size * 2));
                auto mask = vec2::call_step(remainders, vec2(static_cast<float>(checkers_size)));
                auto d = vec2::call_dot(mask, mask);
                return vec4::call_mix(checkers0, checkers1, d);
            }
            else
            {
                using namespace swizzle::detail;

                batch_traits<int32_type>::aligned_storage_type istorage;
                auto ibuffer = reinterpret_cast<int32_t*>(&istorage);
                {
                    int32_type index = (coord.y * pitch + coord.x) * bytes_per_pixel;
                    store_aligned(index, ibuffer);
                }

                batch_traits<float_type>::aligned_storage_type rstorage;
                batch_traits<float_type>::aligned_storage_type gstorage;
                batch_traits<float_type>::aligned_storage_type bstorage;
                batch_traits<float_type>::aligned_storage_type astorage;

                auto rbuffer = reinterpret_cast<float*>(&rstorage);
                auto gbuffer = reinterpret_cast<float*>(&gstorage);
                auto bbuffer = reinterpret_cast<float*>(&bstorage);
                auto abuffer = reinterpret_cast<float*>(&astorage);

                static_for<0, batch_traits<int32_type>::size>([&](size_t i)
                {
                    auto ptr = bytes + ibuffer[i];
                    uint32_t pixel = 0;
                    // TODO: are cache reads aligned?
                    switch (bytes_per_pixel)
                    {
                    case 4: pixel |= (ptr[3] << (3 * 8));
                    case 3: pixel |= (ptr[2] << (2 * 8));
                    case 2: pixel |= (ptr[1] << (1 * 8));
                    case 1: pixel |= (ptr[0] << (0 * 8));
                    }

                    // TODO: a faster cast is possible
                    rbuffer[i] = static_cast<float>((pixel & rmask) >> rshift);
                    gbuffer[i] = static_cast<float>((pixel & gmask) >> gshift);
                    bbuffer[i] = static_cast<float>((pixel & bmask) >> bshift);
                    abuffer[i] = static_cast<float>(amask ? ((pixel & amask) >> ashift) : 255);
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
