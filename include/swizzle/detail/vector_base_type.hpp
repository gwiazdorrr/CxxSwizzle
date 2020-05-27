// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <array>
#include <swizzle/detail/indexed_proxy.hpp>
#include <swizzle/detail/vector_storage.hpp>
#include <swizzle/detail/vector_traits.h>
#include <swizzle/detail/fwd.hpp>


namespace swizzle
{
    namespace detail
    {
        template <typename ScalarType, size_t Size>
        struct vector_base_type_helper
        {
            using data_type = std::array<typename batch_traits<ScalarType>::storage_type, Size>;

            template <size_t... Index>
            struct proxy_generator
            {
                typedef swizzle::detail::indexed_proxy< vector<ScalarType, sizeof...(Index)>, data_type, ScalarType, Index...> type;
            };

            template <size_t Index>
            struct proxy_generator<Index>
            {
                typedef ScalarType type;
            };

            typedef ::swizzle::detail::vector_storage< Size, proxy_generator, data_type > vector_base_type;
        };

        template <typename ScalarType, size_t Size>
        using vector_base_type = typename vector_base_type_helper<ScalarType, Size>::vector_base_type;
    }
}