// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

namespace swizzle
{
    template <typename BoolType, size_t... Index>
    struct vector_;

    namespace detail
    {
        template <typename BoolType, size_t... Index> vector_<BoolType, Index...> make_vector_type_helper(std::index_sequence<Index...>);
    }

    template <typename ScalarType, size_t Size>
    using vector = decltype(detail::make_vector_type_helper<ScalarType>(std::make_index_sequence<Size>{}));
}