// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <swizzle/detail/vector_traits.h>
#include <cmath>
#include <algorithm>

namespace swizzle
{
	namespace glsl
	{
        template <typename ScalarType, size_t... Indices>
        struct vector_;
	}

	namespace detail
	{
		template <class T>
		struct get_vector_type_impl_for_scalar
		{
			typedef ::swizzle::vector_<T, 0> type;
		};


		template <>
		struct get_vector_type_impl<bool>
		{
            typedef ::swizzle::vector_<bool, 0> type;
        };

		template <>
		struct get_vector_type_impl<float> : get_vector_type_impl_for_scalar<float>
		{};

		template <>
		struct get_vector_type_impl<double> : get_vector_type_impl_for_scalar<double>
		{};

		template <>
		struct get_vector_type_impl<signed int> : get_vector_type_impl_for_scalar<signed int>
		{};

		template <>
		struct get_vector_type_impl<unsigned int> : get_vector_type_impl_for_scalar<unsigned int>
		{};

		template <>
		struct get_vector_type_impl<signed short> : get_vector_type_impl_for_scalar<signed short>
		{};

		template <>
		struct get_vector_type_impl<unsigned short> : get_vector_type_impl_for_scalar<unsigned short>
		{};
	}
}

// add missing math functions

namespace swizzle
{
    namespace glsl
    {
        using std::sin;
        using std::cos;
        using std::tan;
        using std::asin;
        using std::acos;
        using std::atan;
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

        inline float isnan(float x)
        {
            // this is because std::isnan is a template and it messes up adl
            return std::isnan(x);
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
    }
}