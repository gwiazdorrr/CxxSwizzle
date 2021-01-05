// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <type_traits>
#include <swizzle/batch_adapters.hpp>
#include <swizzle/detail/fwd.hpp>
#include <swizzle/detail/simd_mask.h>
#include <swizzle/detail/vector_traits.h>

namespace swizzle
{
    using float_type = float_batch<float, bool, 0, 1, 2, 3>;
    using int_type = int_batch<int32_t, bool, 0, 1, 2, 3>;
    using uint_type = uint_batch<uint32_t, bool, 0, 1, 2, 3>;
    using bool_type = bool_batch<bool, 0, 1, 2, 3>;

    // batch types traits definitions
    namespace detail
    {
        template <>
        struct batch_traits<float_type> : batch_traits_builder<
            float,
            float,
            std::alignment_of_v<float>, 1, float_type::size,
            bool_type,
            false, false, true>
        {};

        template <>
        struct batch_traits<int_type> : batch_traits_builder<
            int32_t,
            int32_t,
            std::alignment_of_v<int32_t>, 1, int_type::size,
            bool_type,
            false, true, false>
        {};

        template <>
        struct batch_traits<uint_type> : batch_traits_builder<
            uint32_t,
            uint32_t,
            std::alignment_of_v<uint32_t>, 1, uint_type::size,
            bool_type,
            false, true, false>
        {};

        template <>
        struct batch_traits<bool_type> : batch_traits_builder<
            bool,
            bool,
            std::alignment_of_v<bool>, 1, bool_type::size,
            bool_type,
            true, false, false>
        {};
    }

    // free functions needed for wrappers to work

    template <typename To, typename From>
    inline To batch_cast(const From& value)
    {
        return static_cast<To>(value);
    }

    template <typename T>
    inline T batch_scalar_cast(T value)
    {
        return value;
    }

    inline bool batch_collapse(bool value)
    {
        return value;
    }   

    template <typename T>
    inline void batch_load_aligned(T& value, const T* data)
    {
        value = *data;
    }

    template <typename T>
    inline void batch_store_aligned(const T& value, T* data)
    {
        *data = value;
    }

    template <typename T>
    inline void batch_masked_assign(T& target, const T& src)
    {
        target = src;
    }
}

#include <swizzle/detail/cmath_imports.hpp>
#include <swizzle/detail/setup_common.hpp>
