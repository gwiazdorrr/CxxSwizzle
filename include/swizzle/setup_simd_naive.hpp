// CxxSwizzle (c) 2013-2021 Piotr Gwiazdowski
#pragma once

#include <cstdint>
#include <cstddef>
#include <swizzle/detail/cmath_imports.hpp>
#include <algorithm>

namespace swizzle
{
    // free functions needed for wrappers to work

    template <typename To, typename From>
    inline To batch_cast(const From& value)
    {
        return static_cast<To>(value);
    }

    template <typename T>
    inline T batch_scalar_cast(T value)
    {
        return value;
    }

    inline bool batch_collapse(bool value)
    {
        return value;
    }

    template <typename T>
    inline void batch_load_aligned(T& value, const T* data)
    {
        value = *data;
    }

    template <typename T>
    inline void batch_store_aligned(const T& value, T* data)
    {
        *data = value;
    }

    template <typename T>
    inline void batch_masked_assign(T& target, const T& src)
    {
        target = src;
    }

    inline uint8_t* batch_store_rgba32f_aligned(
        const float r,
        const float g,
        const float b,
        const float a,
        uint8_t* ptr, size_t = 0) noexcept
    {
        float* p = reinterpret_cast<float*>(ptr);
        p[0] = r;
        p[1] = g;
        p[2] = b;
        p[3] = a;
        return ptr + 16;
    }

    inline uint8_t* batch_store_rgba8_aligned(
        const float r,
        const float g,
        const float b,
        const float a,
        uint8_t* ptr, size_t = 0) noexcept
    {
        using std::max;
        using std::min;
        auto ir = max(0, min(255, static_cast<int>(r * 256)));
        auto ig = max(0, min(255, static_cast<int>(g * 256)));
        auto ib = max(0, min(255, static_cast<int>(b * 256)));
        auto ia = max(0, min(255, static_cast<int>(a * 256)));
        uint32_t rgba = ir | (ig << 8) | (ib << 16) | (static_cast<unsigned>(ia) << 24);
        *reinterpret_cast<uint32_t*>(ptr) = rgba;
        return ptr + 4;
    }

    template <typename U, typename T, typename = std::enable_if_t<sizeof(T) == sizeof(U)>>
    inline U bit_cast(T src)
    {
        return *reinterpret_cast<U*>(&src);
    }
}

#include <type_traits>
#include <swizzle/batch_adapters.hpp>
#include <swizzle/detail/fwd.hpp>
#include <swizzle/detail/simd_mask.h>
#include <swizzle/detail/vector_traits.h>

namespace swizzle
{
    using float_type = float_batch<float, bool, 4>;
    using int_type   = int_batch<int32_t, bool, 4>;
    using uint_type  = uint_batch<uint32_t, bool, 4>;
    using bool_type  = bool_batch<bool, 4>;

    // batch types traits definitions
    namespace detail
    {
        template <> struct scalar_type_storage<float_type> { using type = float; };
        template <> struct scalar_type_storage<int_type  > { using type = int32_t; };
        template <> struct scalar_type_storage<uint_type > { using type = uint32_t; };
        template <> struct scalar_type_storage<bool_type > { using type = bool; };
        struct scalar_types_info : scalar_types_info_builder<float_type, int_type, uint_type, bool_type, 1, float_type::size> {};
    }
}

#include <swizzle/detail/setup_common.hpp>
