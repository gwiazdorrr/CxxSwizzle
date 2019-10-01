// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <swizzle/detail/vector_traits.h>
#include <swizzle/detail/fwd.h>
#include <cmath>
#include <algorithm>
#include <cstdint>
#include <swizzle/batch_adapters.hpp>

namespace swizzle
{
    namespace detail
    {
#if !DUPA
        typedef bool simd_bool;
        typedef float simd_float;
        typedef int32_t simd_int32;
        typedef uint32_t simd_uint32;
#else
        template <typename AssignPolicy = default_assign_policy> using simd_bool = bool_batch<bool, AssignPolicy, 0>;
        template <typename AssignPolicy = default_assign_policy> using simd_float = float_batch<float, bool, AssignPolicy, 0>;
        template <typename AssignPolicy = default_assign_policy> using simd_int32 = int_batch<int32_t, bool, AssignPolicy, 0>;
        template <typename AssignPolicy = default_assign_policy> using simd_uint32 = uint_batch<uint32_t, bool, AssignPolicy, 0>;

        template <typename AssignPolicy>
        struct get_vector_type_impl<simd_bool<AssignPolicy>> : default_vector_type_impl<simd_bool<AssignPolicy>>
        {};

        template <typename AssignPolicy>
        struct get_vector_type_impl<simd_float<AssignPolicy>> : default_vector_type_impl<simd_float<AssignPolicy>>
        {};

        template <typename AssignPolicy>
        struct get_vector_type_impl<simd_int32<AssignPolicy>> : default_vector_type_impl<simd_int32<AssignPolicy>>
        {};

        template <typename AssignPolicy>
        struct get_vector_type_impl<simd_uint32<AssignPolicy>> : default_vector_type_impl<simd_uint32<AssignPolicy>>
        {};

        template <typename AssignPolicy>
        struct batch_traits<simd_float<AssignPolicy>> : batch_traits_builder<
            float,
            float,
            4, 1, simd_float<AssignPolicy>::size,
            simd_bool<AssignPolicy>,
            false, false, true>
        {};

        template <typename AssignPolicy>
        struct batch_traits<simd_int32<AssignPolicy>> : batch_traits_builder<
            int32_t,
            int32_t,
            4, 1, simd_int32<AssignPolicy>::size,
            simd_int32<AssignPolicy>,
            false, false, true>
        {};

        template <typename AssignPolicy>
        struct batch_traits<simd_uint32<AssignPolicy>> : batch_traits_builder<
            uint32_t,
            uint32_t,
            4, 1, simd_uint32<AssignPolicy>::size,
            simd_bool<AssignPolicy>,
            false, true, false>
        {};

        template <typename AssignPolicy>
        struct batch_traits<simd_bool<AssignPolicy>> : batch_traits_builder<
            bool,
            bool,
            4, 1, simd_bool<AssignPolicy>::size,
            simd_bool<AssignPolicy>,
            true, false, false>
        {};



#endif
	}


    template <typename To, typename From>
    inline To batch_cast(const From& value)
    {
        return To(value);
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

    template <typename T, typename U>
    inline void batch_load_aligned(T& t, const U* ptr)
    {
        load_aligned(t, ptr);
    }

    template <typename T, typename U>
    inline void batch_store_aligned(const T& t, U* ptr)
    {
        store_aligned(t, ptr);
    }
}

// add missing math functions

namespace swizzle
{
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

    inline void bitcast(float a, int32_t* b)
    {
        *b = *reinterpret_cast<int32_t*>(&a);
    }

    inline void bitcast(float a, uint32_t* b)
    {
        *b = *reinterpret_cast<uint32_t*>(&a);
    }

    inline void bitcast(int32_t a, float* b)
    {
        *b = *reinterpret_cast<float*>(&a);
    }

    inline void bitcast(uint32_t a, float* b)
    {
        *b = *reinterpret_cast<float*>(&a);
    }

    inline float dFdx(float x)
    {
        return 0.0f;
    }

    inline float dFdy(float x)
    {
        return 0.0f;
    }

    inline float fwidth(float x)
    {
        return 0.0f;
    }
 
    inline void load_aligned(uint32_t& target, const uint32_t* ptr)
    {
        target = *ptr;
    }

    inline void load_aligned(int32_t& target, const int32_t* ptr)
    {
        target = *ptr;
    }

    inline void load_aligned(float& target, const float* ptr)
    {
        target = *ptr;
    }

    inline void store_aligned(const uint32_t& target, uint32_t* ptr)
    {
        *ptr = target;
    }

    inline void store_aligned(const int32_t& target, int32_t* ptr)
    {
        *ptr = target;
    }

    inline void store_aligned(const float& target, float* ptr)
    {
        *ptr = target;
    }
}