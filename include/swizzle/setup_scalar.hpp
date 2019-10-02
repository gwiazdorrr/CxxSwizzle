// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <cmath>
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

    // most fath functions are in cmath

    using std::sin;
    using std::cos;
    using std::tan;
    using std::asin;
    using std::acos;
    using std::atan;
    using std::sinh;
    using std::cosh;
    using std::tanh;
    using std::asinh;
    using std::acosh;
    using std::atanh;
    using std::pow;
    using std::exp;
    using std::log;
    using std::exp2;
    using std::log2;
    using std::sqrt;
    using std::abs;
    using std::floor;
    using std::trunc;
    using std::round;
    using std::ceil;
    using std::min;
    using std::max;
    using std::modf;

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

    inline float isnan(float x)
    {
        // this is because std::isnan is a template and it messes up adl
        return std::isnan(x);
    }

    inline float atan(float y, float x)
    {
        return ::std::atan2(y, x);
    }

    inline float isinf(float x)
    {
        // this is because std::isinf is a template and it messes up adl
        return std::isinf(x);
    }

    inline float radians(float x)
    {
        return 180.0f * x / 3.14159265358979323846f;
    }

    inline float degrees(float x)
    {
        return x * 3.14159265358979323846f / 180.0f;
    }

    inline float step(float edge, float x)
    {
        return x > edge ? 1.0f : 0.0f;
    }

    inline float inversesqrt(float x)
    {
        return 1.0f / sqrt(x);
    }

    inline float sign(float x)
    {
        return static_cast<float>((0 < x) - (x < 0));
    }

    inline float fract(float x)
    {
        return x - floor(x);
    }

    inline float mod(float x, float y)
    {
        return x - y * floor(x / y);
    }

    inline float roundEven(float x)
    {
        auto fractPart = fract(x);
        if (fractPart > 0.5f || fractPart < 0.5f)
        {
            return round(x);
        }

        int intValue = static_cast<int>(x);
        auto intPart = static_cast<float>(intValue);

        if ((intValue % 2) == 0)
        {
            return intPart;
        }
        else if (x <= 0) // Work around...
        {
            return intPart - 1.0f;
        }
        else
        {
            return intPart + 1.0f;
        }
    }

    struct partial_derivatives_dont_work_for_scalars
    {};

    float dFdx(partial_derivatives_dont_work_for_scalars);
    float dFdy(partial_derivatives_dont_work_for_scalars);
    float fwidth(partial_derivatives_dont_work_for_scalars);

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

#include <swizzle/detail/setup_common.hpp>
