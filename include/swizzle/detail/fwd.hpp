// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

namespace swizzle
{
    template <typename TScalar, size_t... TIndices>
    struct vector_;

    template <class TScalar, size_t TNumRows, size_t... TColumns >
    struct matrix_;

    template <typename TData, size_t... TIndices>
    struct bool_batch_;

    template <typename TData, typename TBool, size_t... TIndices>
    struct int_batch_;

    template <typename TData, typename TBool, size_t... TIndices>
    struct uint_batch_;

    template <typename TData, typename TBool, size_t... TIndices>
    struct float_batch_;

    namespace detail
    {
        template <template <typename, size_t...> typename TType, typename T1, size_t... TIndices>
        TType<T1, TIndices...> extract_index_sequence_helper(std::index_sequence<TIndices...>);

        template <template <typename, typename, size_t...> typename TType, typename T1, typename T2, size_t... TIndices>
        TType<T1, T2, TIndices...> extract_index_sequence_helper(std::index_sequence<TIndices...>);

        template <template <typename, size_t, size_t...> typename TType, typename T1, size_t T2, size_t... TIndices>
        TType<T1, T2, TIndices...> extract_index_sequence_helper(std::index_sequence<TIndices...>);
    }

    template <typename TScalar, size_t TSize>
    using vector = decltype(detail::extract_index_sequence_helper<vector_, TScalar>(std::make_index_sequence<TSize>{}));

    template <class TScalar, size_t TNumColumns, size_t TNumRows>
    using matrix = decltype(detail::extract_index_sequence_helper<matrix_, TScalar, TNumRows>(std::make_index_sequence<TNumColumns>{}));

    template <typename TData, size_t TSize>
    using bool_batch = decltype(detail::extract_index_sequence_helper<bool_batch_, TData>(std::make_index_sequence<TSize>{}));

    template <typename TData, typename TBool, size_t TSize>
    using int_batch = decltype(detail::extract_index_sequence_helper<int_batch_, TBool, TData>(std::make_index_sequence<TSize>{}));

    template <typename TData, typename TBool, size_t TSize>
    using uint_batch = decltype(detail::extract_index_sequence_helper<uint_batch_, TBool, TData>(std::make_index_sequence<TSize>{}));

    template <typename TData, typename TBool, size_t TSize>
    using float_batch = decltype(detail::extract_index_sequence_helper<float_batch_, TBool, TData>(std::make_index_sequence<TSize>{}));

    template <typename TFloat, typename TInt32, typename TUint32>
    struct sampler_generic;
}