// CxxSwizzle (c) 2013-2021 Piotr Gwiazdowski
#pragma once

#include <cstdint>

#include <swizzle/detail/utils.hpp>
#include <swizzle/detail/vector_traits.hpp>
#include <swizzle/detail/cmath_imports.hpp>

namespace swizzle
{
    using float_type = float;
    using int_type = int32_t;
    using uint_type = uint32_t;
    using bool_type = bool;

    //struct partial_derivatives_dont_work_for_scalars
    //{};



    // aux functions

    template <typename U, typename T, typename = std::enable_if_t<std::is_fundamental_v<T> && std::is_fundamental_v<U> && sizeof(T) == sizeof(U)>>
    inline U bit_cast(T src)
    {
        return *reinterpret_cast<U*>(&src);
    }

    template <typename T, typename = std::enable_if_t<std::is_fundamental_v<T>>>
    inline void load_aligned(T& value, const void* ptr)
    {
        value = *reinterpret_cast<const T*>(ptr);
    }

    template <typename T, typename = std::enable_if_t<std::is_fundamental_v<T>>>
    inline void store_aligned(const T& value, void* ptr)
    {
        *reinterpret_cast<T*>(ptr) = value;
    }

    inline void store_rgba8_aligned(float r, float g, float b, float a, uint8_t* ptr, size_t) {

        auto ir = max(0, min(255, static_cast<int>(r * 256)));
        auto ig = max(0, min(255, static_cast<int>(g * 256)));
        auto ib = max(0, min(255, static_cast<int>(b * 256)));
        auto ia = max(0, min(255, static_cast<int>(a * 256)));
        uint32_t rgba = ir | (ig << 8) | (ib << 16) | (static_cast<unsigned>(ia) << 24);
        *reinterpret_cast<uint32_t*>(ptr) = rgba;
    }

    inline void store_rgba32f_aligned(float r, float g, float b, float a, uint8_t* ptr, size_t) {
        float* p = reinterpret_cast<float*>(ptr);
        p[0] = r;
        p[1] = g;
        p[2] = b;
        p[3] = a;
    }

    namespace detail
    {
        using scalar_types_info = default_scalar_types;// scalar_types_info_builder<float_type, int_type, uint_type, bool_type, 1, 1> {};
    }
}

#include <swizzle/detail/setup_common.hpp>
