// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <cstdint>

#include <swizzle/detail/utils.h>
#include <swizzle/detail/vector_traits.h>

namespace swizzle
{
    using float_type = float;
    using int_type = int32_t;
    using uint_type = uint32_t;
    using bool_type = bool;
    using byte_type = uint8_t;

    //struct partial_derivatives_dont_work_for_scalars
    //{};

    inline float dFdx(float) { return 0; };
    inline float dFdy(float) { return 0; };
    inline float fwidth(float) { return 0; };

    // TODO: propert types
    inline int min(int a, int b)
    {
        return a < b ? a : b;
    }

    inline int max(int a, int b)
    {
        return a > b ? a : b;
    }

    inline int min(unsigned a, unsigned b)
    {
        return a < b ? a : b;
    }

    inline int max(unsigned a, unsigned b)
    {
        return a > b ? a : b;
    }

    inline float min(float a, float b)
    {
        // NaN bug "simulation": https://www.shadertoy.com/view/4tsyzf
        return a < b || (b != b) ? a : b;
    }

    inline float max(float a, float b)
    {
        // NaN bug "simulation": https://www.shadertoy.com/view/4tsyzf
        return a > b || (b != b) ? a : b;
    }

    // aux functions

    template <typename T, typename U, typename = std::enable_if_t<std::is_fundamental_v<T> && std::is_fundamental_v<U> && sizeof(T) == sizeof(U)>>
    inline void bitcast(T src, U& target)
    {
        target = *reinterpret_cast<U*>(src);
    }

    template <typename T, typename = std::enable_if_t<std::is_fundamental_v<T>>>
    inline void load_aligned(T& value, const T* ptr)
    {
        value = *ptr;
    }

    template <typename T, typename = std::enable_if_t<std::is_fundamental_v<T>>>
    inline void store_aligned(const T& value, T* ptr)
    {
        *ptr = value;
    }

    inline void store_rgba32_aligned(float r, float g, float b, float a, uint8_t* ptr, size_t) {

        auto ir = max(0, min(255, static_cast<int>(floor(r * 256))));
        auto ig = max(0, min(255, static_cast<int>(floor(g * 256))));
        auto ib = max(0, min(255, static_cast<int>(floor(b * 256))));
        auto ia = max(0, min(255, static_cast<int>(floor(a * 256))));
        uint32_t rgba = ir | (ig << 8) | (ib << 16) | (static_cast<unsigned>(ia) << 24);
        *reinterpret_cast<uint32_t*>(ptr) = rgba;
    }

    //inline void load_r8g8b8a8_aligned(const void* ptr, float& r, float& g, float& b, float& a) {
    //    uint32_t rgba = *reinterpret_cast<const uint32_t*>(ptr);
    //    r = ((rgba >> 0)  & 0xFF) / 255.0f;
    //    g = ((rgba >> 8)  & 0xFF) / 255.0f;
    //    b = ((rgba >> 16) & 0xFF) / 255.0f;
    //    a = ((rgba >> 24) & 0xFF) / 255.0f;
    //}
}

#include <swizzle/detail/cmath_imports.hpp>
#include <swizzle/detail/setup_common.hpp>
