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
    enum class texture_formats : int
    {
        a8 =                    1,
        r8g8b8a8 =              4,
        r32g32b32a32_float =    16,
    };

    struct sampler_generic_data
    {
        static sampler_generic_data make(const void* p, int width, int height, int pitch, texture_formats format) 
        {
            return {
                reinterpret_cast<const uint8_t*>(p),
                width,
                height,
                pitch,
                1.0f / width,
                1.0f / height,
                format
            };
        }

        const uint8_t* bytes = nullptr;

        int width = 256;
        int height = 256; 
        int pitch_bytes = 0;

        float pixel_w;
        float pixel_h;

        

        texture_formats format;
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
        std::tuple<IntType, IntType> uv_to_icoord(SomeFloatType u, SomeFloatType v, const sampler_generic_data* data) const 
        {
            using std::min;
            using std::max;
            auto ix = static_cast<IntType>(u * static_cast<float>(data->width));
            auto iy = static_cast<IntType>(v * static_cast<float>(data->height));
            detail::nonmasked(ix) = min(IntType(data->width - 1), ix);
            detail::nonmasked(iy) = min(IntType(data->height - 1), iy);
            // lazy Nan workaround
            detail::nonmasked(ix) = max(IntType(0), ix);
            detail::nonmasked(iy) = max(IntType(0), iy);
            return std::make_tuple(ix, iy);
        }

        template <typename IntType>
        std::tuple<IntType, IntType> clamp_icoords(IntType ix, IntType iy, const sampler_generic_data* data) const
        {
            using std::min;
            using std::max;
            detail::nonmasked(ix) = max(IntType(0), min(IntType(data->width - 1), ix));
            detail::nonmasked(iy) = max(IntType(0), min(IntType(data->height - 1), iy));
            return std::make_tuple(ix, iy);
        }


        template <typename IntType>
        IntType icoord_to_index(IntType x, IntType y, const sampler_generic_data* data) const
        {
            if (vflip)
            {
                return (data->height - 1 - y) * data->pitch_bytes + x * static_cast<int>(data->format);
            }
            else
            {
                return y * data->pitch_bytes + x * static_cast<int>(data->format);
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

            if (data->format == texture_formats::r32g32b32a32_float) {
                return std::make_tuple(
                    reinterpret_cast<const float*>(ptr)[0],
                    reinterpret_cast<const float*>(ptr)[1],
                    reinterpret_cast<const float*>(ptr)[2],
                    reinterpret_cast<const float*>(ptr)[3]
                );
            }
            else if (data->format == texture_formats::a8) 
            {
                auto r = ptr[0] / 255.0f;
                return std::make_tuple(r, r, r, r);
            }
            else
            {
                uint32_t pixel = *reinterpret_cast<const uint32_t*>(ptr);
                auto r = ptr[0];
                auto g = ptr[1];
                auto b = ptr[2];
                auto a = ptr[3];

                return std::make_tuple(
                    r / 255.0f,
                    g / 255.0f,
                    b / 255.0f,
                    a / 255.0f
                );
            }
        }

        vec4 sample(const vec3& _coord) const
        {
            // this shit is going to be problematic...
            vec3 coord = coord.call_normalize(_coord);

            using namespace swizzle::detail;

            alignas(scalar_traits<float_type>::align) float px[scalar_traits<float_type>::size];
            alignas(scalar_traits<float_type>::align) float py[scalar_traits<float_type>::size];
            alignas(scalar_traits<float_type>::align) float pz[scalar_traits<float_type>::size];
            alignas(scalar_traits<float_type>::align) float pr[scalar_traits<float_type>::size];
            alignas(scalar_traits<float_type>::align) float pg[scalar_traits<float_type>::size];
            alignas(scalar_traits<float_type>::align) float pb[scalar_traits<float_type>::size];
            alignas(scalar_traits<float_type>::align) float pa[scalar_traits<float_type>::size];

            store_aligned(coord.x, px);
            store_aligned(coord.y, py);
            store_aligned(coord.z, pz);

            static_for<0, scalar_traits<float_type>::size>([&](size_t i)
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

        vec4 fetch(const ivec2& coord) const
        {
            return fetch_no_bounds_check(clamp_icoords(coord.x, coord.y, data));
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

                vec4 x0y0 = fetch_no_bounds_check(std::get<0>(icoord), std::get<1>(icoord));
                vec4 x0y1 = fetch_no_bounds_check(std::get<0>(icoord), std::get<1>(icoord_diag));
                vec4 x1y0 = fetch_no_bounds_check(std::get<0>(icoord_diag), std::get<1>(icoord));
                vec4 x1y1 = fetch_no_bounds_check(std::get<0>(icoord_diag), std::get<1>(icoord_diag));

                vec4 y0 = vec4::call_mix(x0y0, x1y0, error_abs.x);
                vec4 y1 = vec4::call_mix(x0y1, x1y1, error_abs.x);
                return vec4::call_mix(y0, y1, error_abs.y);
            }
            else
            {
                return fetch_no_bounds_check(uv_to_icoord<int32_type>(uv.x, uv.y, data));
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

        vec4 fetch_no_bounds_check(const std::tuple<int32_type, int32_type>& coord) const
        {
            return fetch_no_bounds_check(std::get<0>(coord), std::get<1>(coord));
        }

        vec4 fetch_no_bounds_check(const int32_type& x, const int32_type& y) const
        {
            if (!data->bytes)
            {
                return vec4(0.0f, 0.0f, 0.0f, 1.0f);
            }
            else
            {
                using namespace swizzle::detail;

                alignas(scalar_traits<int32_type>::align) int ibuffer[scalar_traits<int32_type>::size];
                {
                    int32_type index = icoord_to_index(x, y, data);
                    store_aligned(index, ibuffer);
                }
                
                alignas(scalar_traits<float_type>::align) float pr[scalar_traits<float_type>::size];
                alignas(scalar_traits<float_type>::align) float pg[scalar_traits<float_type>::size];
                alignas(scalar_traits<float_type>::align) float pb[scalar_traits<float_type>::size];
                alignas(scalar_traits<float_type>::align) float pa[scalar_traits<float_type>::size];

                fetch_pixels(ibuffer, pr, pg, pb, pa);

                vec4 result;
                load_aligned(result.r, pr);
                load_aligned(result.g, pg);
                load_aligned(result.b, pb);
                load_aligned(result.a, pa);
                return result;
            }
        }

        template <size_t TCount>
        void fetch_pixels(int(&index)[TCount], float(&target_r)[TCount], float(&target_g)[TCount], float(&target_b)[TCount], float(&target_a)[TCount]) const
        {
            if (data->format == texture_formats::r32g32b32a32_float)
            {
                detail::static_for<0, TCount>([&](size_t i) -> void {
                    auto pf = reinterpret_cast<const float*>(data->bytes + index[i]);
                    target_r[i] = pf[0];
                    target_g[i] = pf[1];
                    target_b[i] = pf[2];
                    target_a[i] = pf[3];
                    });
            }
            else if (data->format == texture_formats::a8)
            {
                detail::static_for<0, TCount>([&](size_t i) -> void {
                    auto a = *(data->bytes + index[i]) / 255.0f;
                    target_r[i] = a;
                    target_g[i] = a;
                    target_b[i] = a;
                    target_a[i] = a;
                    });
            }
            else
            {
                detail::static_for<0, TCount>([&](size_t i) -> void {
                    auto ptr = data->bytes + index[i];
                    target_r[i] = ptr[0] / 255.0f;
                    target_g[i] = ptr[1] / 255.0f;
                    target_b[i] = ptr[2] / 255.0f;
                    target_a[i] = ptr[3] / 255.0f;
                    });
            }
        }
    };
}
