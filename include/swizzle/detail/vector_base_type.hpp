// CxxSwizzle (c) 2013-2021 Piotr Gwiazdowski
#pragma once

#include <array>
#include <swizzle/detail/indexed_swizzle.hpp>
#include <swizzle/detail/vector_storage.hpp>
#include <swizzle/detail/vector_traits.h>
#include <swizzle/detail/fwd.hpp>


namespace swizzle
{
    namespace detail
    {
        template <typename TScalar, size_t TSize>
        struct vector_base_type_helper
        {
            using data_type = std::array<typename scalar_traits<TScalar>::storage_type, TSize>;

            template <size_t... TIndices>
            struct proxy_generator
            {
                typedef swizzle::detail::indexed_swizzle< vector<TScalar, sizeof...(TIndices)>, data_type, TScalar, TIndices...> type;
            };

            template <size_t TIndex>
            struct proxy_generator<TIndex>
            {
                typedef TScalar type;
            };

            typedef ::swizzle::detail::vector_storage< TSize, proxy_generator, data_type > vector_base_type;
        };

        template <typename TScalar, size_t TSize>
        using vector_base_type = typename vector_base_type_helper<TScalar, TSize>::vector_base_type;
    }
}