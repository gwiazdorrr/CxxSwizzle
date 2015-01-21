// CxxSwizzle
// Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <array>
#include <swizzle/detail/indexed_proxy.h>
#include <swizzle/detail/vector_base.h>

namespace swizzle
{
    namespace glsl
    {
        template < class ScalarType, size_t Size >
        class vector;

        //! A type that simplifies creating proxies factories for the vector.
        template <class ScalarType, size_t Size>
        struct vector_helper
        {
            //! These can be incomplete types at this point.
            typedef std::array<ScalarType, Size> data_type;

            template <size_t... indices>
            struct proxy_generator
            {
                typedef detail::indexed_proxy< vector<ScalarType, sizeof...(indices)>, data_type, indices...> type;
            };

            //! A factory of 1-component proxies.
            template <size_t x>
            struct proxy_generator<x>
            {
                typedef ScalarType type;
            };

            typedef detail::vector_base< Size, proxy_generator, data_type > base_type;

            static ScalarType convert_other_scalar_type(const ScalarType& other)
            {
                return other;
            }

            typedef ScalarType masked_scalar_type;
        };
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

        template <class ScalarType, size_t Size>
        struct get_vector_type_impl< ::swizzle::glsl::vector<ScalarType, Size> >
        {
            typedef ::swizzle::glsl::vector<ScalarType, Size> type;
        };
    }
}

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
}