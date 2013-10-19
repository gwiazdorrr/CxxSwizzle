//  CxxSwizzle
//  Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr.os@gmail.com>
#pragma once

#include <type_traits>
#include <swizzle/detail/vector_traits.h>

namespace swizzle
{
    namespace naive
    {
        template <class TScalar, size_t Size>
        class vector_adapter;
    }

    namespace detail
    {
        template <class T>
        struct get_vector_type_impl_for_scalar
        {
            typedef naive::vector_adapter< T, 1 > type;
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
        struct get_vector_type_impl<int> : get_vector_type_impl_for_scalar<int>
        {};

        template <>
        struct get_vector_type_impl<unsigned short> : get_vector_type_impl_for_scalar<unsigned short>
        {};

        template <class TScalar, size_t Size>
        struct get_vector_type_impl< naive::vector_adapter<TScalar, Size> >
        {
            typedef naive::vector_adapter<TScalar, Size> type;
        };
    }
}