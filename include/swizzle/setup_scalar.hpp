// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <algorithm>
#include <cstdint>

#include <swizzle/detail/utils.h>
#include <swizzle/detail/vector_traits.h>

namespace swizzle
{
    using float_type = float;
    using int_type = int32_t;
    using uint_type = uint32_t;
    using bool_type = bool;

    //struct partial_derivatives_dont_work_for_scalars
    //{};

    inline float dFdx(float) { return 0; };
    inline float dFdy(float) { return 0; };
    inline float fwidth(float) { return 0; };

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
}

#include <swizzle/detail/cmath_imports.hpp>
#include <swizzle/detail/setup_common.hpp>
