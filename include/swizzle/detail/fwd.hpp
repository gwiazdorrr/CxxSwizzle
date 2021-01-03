// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

namespace swizzle
{
    template <typename TScalar, size_t... TIndices>
    struct vector_;

    template <class TScalar, size_t TNumRows, size_t... TColumns >
    struct matrix_;

    namespace detail
    {
        template <typename TScalar, size_t... TIndices> 
        vector_<TScalar, TIndices...> make_vector_type_helper(std::index_sequence<TIndices...>);

        template <class TScalar, size_t TNumRows, size_t... TColumns>
        matrix_<TScalar, TNumRows, TColumns...> make_matrix_type_helper(std::index_sequence<TColumns...>);
    }

    template <typename TScalar, size_t TSize>
    using vector = decltype(detail::make_vector_type_helper<TScalar>(std::make_index_sequence<TSize>{}));

    template <class TScalar, size_t TNumColumns, size_t TNumRows>
    using matrix = decltype(detail::make_matrix_type_helper<TScalar, TNumRows>(std::make_index_sequence<TNumColumns>{}));

    template <typename TFloat, typename TInt32, typename TUint32>
    struct naive_sampler_generic;
}