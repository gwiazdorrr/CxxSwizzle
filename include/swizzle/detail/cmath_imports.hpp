// CxxSwizzle (c) 2013-2021 Piotr Gwiazdowski
#pragma once

#include <type_traits>
#include <cstdint>
#include <swizzle/detail/utils.h>
#include <swizzle/detail/vector_traits.h>
#include <cmath>

namespace swizzle
{
    // most functions are in cmath

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
    using std::modf;

    inline bool isnan(float x)
    {
        // this is because std::isnan is a template and it messes up adl
        return std::isnan(x);
    }

    inline float atan(float y, float x)
    {
        return ::std::atan2(y, x);
    }

    inline bool isinf(float x)
    {
        // this is because std::isinf is a template and it messes up adl
        return std::isinf(x);
    }

    inline float degrees(float x)
    {
        return 180.0f * x / 3.14159265358979323846f;
    }

    inline float radians(float x)
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

        int32_t intValue = static_cast<int32_t>(x);
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

    inline float mix(float a, float b, bool val)
    {
        return val ? b : a;
    }

    inline int32_t min(int32_t a, int32_t b)
    {
        return a < b ? a : b;
    }

    inline int32_t max(int32_t a, int32_t b)
    {
        return a > b ? a : b;
    }

    inline int32_t min(uint32_t a, uint32_t b)
    {
        return a < b ? a : b;
    }

    inline uint32_t max(uint32_t a, uint32_t b)
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

    inline float dFdx(float) { return 0; };
    inline float dFdy(float) { return 0; };
    inline float fwidth(float) { return 0; };
}