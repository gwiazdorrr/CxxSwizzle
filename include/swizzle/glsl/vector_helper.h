// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <array>
#include <swizzle/detail/indexed_proxy.h>
#include <swizzle/detail/vector_storage.h>
#include <swizzle/detail/vector_traits.h>
#include <swizzle/detail/fwd.h>


namespace swizzle
{
    namespace detail
    {
        template<typename ScalarType, size_t Size, typename DataType, typename BoolType = bool >
        struct vector_build_info_base
        {
            using data_type = DataType;

            template <size_t... Index>
            struct proxy_generator
            {
                typedef swizzle::detail::indexed_proxy< glsl::vector<ScalarType, sizeof...(Index)>, data_type, Index...> type;
            };

            template <size_t Index>
            struct proxy_generator<Index>
            {
                typedef ScalarType type;
            };

            typedef ::swizzle::detail::vector_storage< Size, proxy_generator, data_type > base_type;

            typedef BoolType bool_type;

            static const bool is_bool = is_scalar_bool_v<ScalarType>;
            static const bool is_integral = is_scalar_integral_v<ScalarType>;
            static const bool is_floating_point = is_scalar_floating_point_v<ScalarType>;
            static const bool is_number = is_integral || is_floating_point;
        };

        template <typename ScalarType, size_t Size>
        struct vector_build_info : vector_build_info_base<ScalarType, Size, std::array<ScalarType, Size>, bool>
        {};
    }
}