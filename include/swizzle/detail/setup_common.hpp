// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <type_traits>
#include <cstdint>
#include <swizzle/detail/utils.h>
#include <swizzle/detail/vector_traits.h>

namespace swizzle
{
    namespace detail
    {
        template <> struct get_vector_type_impl<float_type> : default_vector_type_impl<float_type> {};
        template <> struct get_vector_type_impl<int_type> : default_vector_type_impl<int_type> {};
        template <> struct get_vector_type_impl<uint_type> : default_vector_type_impl<uint_type> {};
        template <> struct get_vector_type_impl<bool_type> : default_vector_type_impl<bool_type> {};

        // now make sure fundamentals get the right treatment too
        template <> struct get_vector_type_impl<only_if<!std::is_same_v<float_type, float>, float, __LINE__>>   : get_vector_type_impl<float_type> {};
        template <> struct get_vector_type_impl<only_if<!std::is_same_v<float_type, double>, double, __LINE__>> : get_vector_type_impl<float_type> {};

        template <> struct get_vector_type_impl<only_if<!std::is_same_v<int_type, int16_t>, int16_t, __LINE__>> : get_vector_type_impl<int_type> {};
        template <> struct get_vector_type_impl<only_if<!std::is_same_v<int_type, int32_t>, int32_t, __LINE__>> : get_vector_type_impl<int_type> {};
        template <> struct get_vector_type_impl<only_if<!std::is_same_v<int_type, int64_t>, int64_t, __LINE__>> : get_vector_type_impl<int_type> {};

        template <> struct get_vector_type_impl<only_if<!std::is_same_v<uint_type, uint16_t>, uint16_t, __LINE__>> : get_vector_type_impl<uint_type> {};
        template <> struct get_vector_type_impl<only_if<!std::is_same_v<uint_type, uint32_t>, uint32_t, __LINE__>> : get_vector_type_impl<uint_type> {};
        template <> struct get_vector_type_impl<only_if<!std::is_same_v<uint_type, uint64_t>, uint64_t, __LINE__>> : get_vector_type_impl<uint_type> {};
    }
}