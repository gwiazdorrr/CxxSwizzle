// CxxSwizzle
// Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <swizzle/detail/vector_traits.h>

namespace swizzle
{
    namespace glsl
    {
        namespace naive
        {
            template <class ScalarType, size_t Size>
            class vector;
        }
    }

    namespace detail
    {
        template <class T>
        struct get_vector_type_impl_for_scalar
        {
            typedef glsl::naive::vector<T, 1> type;
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
        struct get_vector_type_impl< glsl::naive::vector<ScalarType, Size> >
        {
            typedef glsl::naive::vector<ScalarType, Size> type;
        };
    }
}