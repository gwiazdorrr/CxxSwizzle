// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

namespace swizzle
{
    template <typename ScalarType, size_t... Index>
    struct vector_;

    template <class ScalarType, size_t NumRows, size_t... Columns >
    struct matrix_;

    namespace detail
    {
        template <typename ScalarType, size_t... Index> 
        vector_<ScalarType, Index...> make_vector_type_helper(std::index_sequence<Index...>);

        template <class ScalarType, size_t NumRows, size_t... Columns>
        matrix_<ScalarType, NumRows, Columns...> make_matrix_type_helper(std::index_sequence<Columns...>);
    }

    template <typename ScalarType, size_t Size>
    using vector = decltype(detail::make_vector_type_helper<ScalarType>(std::make_index_sequence<Size>{}));

    template <class ScalarType, size_t NumRows, size_t NumColumns>
    using matrix = decltype(detail::make_matrix_type_helper<ScalarType, NumRows>(std::make_index_sequence<NumColumns>{}));

    template <typename FloatType, typename Int32Type, typename UInt32Type>
    struct naive_sampler_generic;
}