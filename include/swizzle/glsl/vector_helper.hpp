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
        struct vector_build_info_base
        {
            using data_type = std::array<typename batch_traits<ScalarType>::storage_type, Size>;

            template <size_t... Index>
            struct proxy_generator
            {
                typedef swizzle::detail::indexed_proxy< vector<ScalarType, sizeof...(Index)>, data_type, Index...> type;
            };

            template <size_t Index>
            struct proxy_generator<Index>
            {
                typedef ScalarType type;
            };

            typedef ::swizzle::detail::vector_storage< Size, proxy_generator, data_type > base_type;

            using bool_type = typename batch_traits<ScalarType>::bool_type;

            static const bool is_bool = batch_traits<ScalarType>::is_bool;
            static const bool is_integral = batch_traits<ScalarType>::is_integral;
            static const bool is_floating_point = batch_traits<ScalarType>::is_floating_point;
            static const bool is_number = is_integral || is_floating_point;
        };

        template <typename ScalarType, size_t Size>
        struct vector_build_info : vector_build_info_base<ScalarType, Size>
        {};
    }
}