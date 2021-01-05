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
    struct sampler_generic_data
    {
        static sampler_generic_data make_float32(const void* p, int width, int height, int pitch)
        {
            sampler_generic_data result;
            result.bytes = reinterpret_cast<const uint8_t*>(p);
            result.width = width;
            result.height = height;
            result.pixel_w = 1.0f / width;
            result.pixel_h = 1.0f / height;
            result.pitch_bytes = static_cast<unsigned>(pitch);
            result.bytes_per_pixel = sizeof(float) * 4;
            result.is_floating_point = true;
            return std::move(result);
        }

        static sampler_generic_data make_rgba(const void* p, int width, int height, int pitch, int bpp,
            uint8_t rshift, uint8_t gshift, uint8_t bshift, uint8_t ashift,
            uint32_t rmask, uint32_t gmask, uint32_t bmask, uint32_t amask)
        {
            sampler_generic_data result;

            result.bytes = reinterpret_cast<const uint8_t*>(p);
            result.width = width;
            result.height = height;
            result.pixel_w = 1.0f / width;
            result.pixel_h = 1.0f / height;
            result.pitch_bytes = pitch;
            result.bytes_per_pixel = bpp;

            result.rshift = rshift;
            result.gshift = gshift;
            result.bshift = bshift;
            result.ashift = ashift;

            result.rmask = rmask;
            result.gmask = gmask;
            result.bmask = bmask;
            result.amask = amask;

            return std::move(result);
        }

        int width = 256;
        int height = 256; 

        float pixel_w;
        float pixel_h;

        uint32_t rmask = 0;
        uint32_t gmask = 0;
        uint32_t bmask = 0;
        uint32_t amask = 0;

        uint8_t rshift = 0;
        uint8_t gshift = 0;
        uint8_t bshift = 0;
        uint8_t ashift = 0;

        const uint8_t* bytes = nullptr;
        int bytes_per_pixel = 0;
        int pitch_bytes = 0;

        bool is_floating_point = false;
    };

    enum class texture_filter_modes {
        nearest,
        linear,
    };

    enum class texture_wrap_modes {
        clamp,
        repeat,
        mirror_repeat,
    };


    //! A really naive implemenation of a sampler
    template <typename TFloat, typename TInt32, typename TUint32>
    struct sampler_generic
    {
        using this_type = sampler_generic;

        using float_type = TFloat;
        using uint32_type = TUint32;
        using int32_type = TInt32;

        using vec2 = vector<float_type, 2>;
        using vec3 = vector<float_type, 3>;
        using vec4 = vector<float_type, 4>;
        using ivec2 = vector<int32_type, 2>;

        static inline const vec4 checkers0 = vec4(0.6f);
        static inline const vec4 checkers1 = vec4(0.9f);
        static inline const sampler_generic_data empty_data = {};

        const sampler_generic_data* data = &empty_data;
        size_t face_count = 1;

        texture_wrap_modes wrap_mode = texture_wrap_modes::repeat;
        texture_filter_modes filter_mode = texture_filter_modes::nearest;
        bool vflip  = false;


        // functions
        // 8.9

        inline friend vec4 texture(const this_type& sampler, const vec2& p)
        {
            return sampler.sample(p);
        }

        inline friend vec4 texture(const this_type& sampler, const vec2& p, const float_type& bias)
        {
            return sampler.sample(p);
        }

        inline friend vec4 texture(const this_type& sampler, const vec3& p)
        {
            return sampler.sample(p);
        }

        inline friend vec4 texture(const this_type& sampler, const vec3& p, const float_type& bias)
        {
            return sampler.sample(p);
        }


        inline friend vec4 textureLod(const this_type& sampler, const vec2& p, const float_type& lod)
        {
            return sampler.sample(p);
        }

        inline friend vec4 textureLod(const this_type& sampler, const vec3& p, const float_type& lod)
        {
            return sampler.sample(p);
        }


        inline friend vec4 texelFetch(const this_type& sampler, const ivec2& p, int32_type lod)
        {
            return sampler.fetch(p);
        }



        inline friend vec4 textureGrad(const this_type& sampler, const vec2& p, const vec2& dPdx, const vec2& dPdy)
        {
            return sampler.sample(p);
        }

        inline friend vec4 textureGrad(const this_type& sampler, const vec3& p, const vec3& dPdx, const vec3& dPdy)
        {
            return sampler.sample(p);
        }


        inline friend ivec2 textureSize(const this_type& sampler, int)
        {
            return { sampler.data->width, sampler.data->height };
        }
    private:

        template <typename IntType, typename SomeFloatType>
        std::tuple<IntType, IntType> uv_to_icoord(SomeFloatType u, SomeFloatType v, const sampler_generic_data* data) const {
            using std::min;
            auto ix = static_cast<IntType>(u * static_cast<float>(data->width));
            auto iy = static_cast<IntType>(v * static_cast<float>(data->height));
            detail::nonmasked(ix) = min(IntType(data->width - 1), ix);
            detail::nonmasked(iy) = min(IntType(data->height - 1), iy);
            return std::make_tuple(ix, iy);
        }

        template <typename IntType>
        IntType icoord_to_index(IntType x, IntType y, const sampler_generic_data* data) const
        {
            if (vflip)
            {
                return (data->height - 1 - y) * data->pitch_bytes + x * data->bytes_per_pixel;
            }
            else
            {
                return y * data->pitch_bytes + x * data->bytes_per_pixel;
            }
        }


        std::tuple<float, float, float, float> fetch_pixel(int index, const sampler_generic_data* data) const
        {
            auto ptr = data->bytes + index;
            if (index < 0 || index > data->height * data->pitch_bytes) 
            {
                // TODO: verify what's shadertoy's take on oob fetches
                return std::make_tuple(0.0f, 0.0f, 0.0f, 1.0f);
            }

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
                    float s = x > 0.0f ? 1.0f : -1.0f;
                    face_index = 0 + (s > 0 ? 0 : 1);
                    u = -1 * z / x;
                    v =  s * y / x;
                }
                else if (ay >= az && ay >= az) 
                {
                    float s = y > 0 ? 1.0f : -1.0f;
                    face_index = 2 + (s > 0 ? 0 : 1);
                    u =  s * x / y;
                    v = -s * z / y;
                }
                else
                {
                    float s = z > 0 ? 1.0f : -1.0f;
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
            vec2 uv = apply_wrap_mode(coord);

            if (filter_mode == texture_filter_modes::linear)
            {
                vec2 perfect_coords = vec2(uv.x * static_cast<float>(data->width), uv.y * static_cast<float>(data->height));
                vec2 error = vec2::call_fract(perfect_coords) - 0.5f;
                vec2 error_sign = vec2::call_sign(error);
                vec2 error_abs = vec2::call_abs(error);

                // uv coordinate of a closest diagonal texel
                vec2 uv_diag = apply_wrap_mode(coord + vec2(error_sign.x * data->pixel_w, error_sign.y * data->pixel_h));

                auto icoord = uv_to_icoord<int32_type>(uv.x, uv.y, data);
                auto icoord_diag = uv_to_icoord<int32_type>(uv_diag.x, uv_diag.y, data);

                vec4 x0y0 = fetch(std::get<0>(icoord), std::get<1>(icoord));
                vec4 x0y1 = fetch(std::get<0>(icoord), std::get<1>(icoord_diag));
                vec4 x1y0 = fetch(std::get<0>(icoord_diag), std::get<1>(icoord));
                vec4 x1y1 = fetch(std::get<0>(icoord_diag), std::get<1>(icoord_diag));

                vec4 y0 = vec4::call_mix(x0y0, x1y0, error_abs.x);
                vec4 y1 = vec4::call_mix(x0y1, x1y1, error_abs.x);
                return vec4::call_mix(y0, y1, error_abs.y);
            }
            else
            {
                ivec2 icoord;
                std::tie(icoord.x, icoord.y) = uv_to_icoord<int32_type>(uv.x, uv.y, data);
                return fetch(icoord);
            }
        }

        vec2 apply_wrap_mode(vec2 coord) const
        {
            switch (wrap_mode)
            {
            case texture_wrap_modes::repeat:
                return vec2::call_mod(coord, 1.0f);
            case texture_wrap_modes::mirror_repeat:
                return vec2::call_abs(vec2::call_mod(coord - 1.0f, 2.0f) - 1.0f);
            default:
                return vec2::call_clamp(coord, 0.0f, 1.0f);
            }
        }

        vec4 fetch(const std::tuple<int32_type, int32_type>& coord) const
        {
            return fetch(std::get<0>(coord), std::get<1>(coord));
        }

        vec4 fetch(const int32_type& x, const int32_type& y) const
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
                    int32_type index = icoord_to_index(x, y, data);
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
