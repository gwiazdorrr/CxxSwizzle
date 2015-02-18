// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <swizzle/detail/vector_traits.h>
#include <cmath>

namespace swizzle
{
    namespace glsl
    {
        template < class ScalarType, size_t Size >
        class vector;
    }

    namespace detail
    {
        template <class T>
        struct get_vector_type_impl_for_scalar
        {
            typedef ::swizzle::glsl::vector<T, 1> type;
        };


        template <>
        struct get_vector_type_impl<bool> : get_vector_type_impl_for_scalar<bool>
        {};

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

namespace std
{
    inline float step(float edge, float  x)
    {
        return x > edge ? 1.0f : 0.0f;
    }

    inline float rsqrt(float x)
    {
        return 1.0f / sqrt(x);
    }

    inline float sign(float x)
    {
        return (0 < x) - (x < 0);
    }

    inline float fract(float x)
    {
        return x - floor(x);
    }
}