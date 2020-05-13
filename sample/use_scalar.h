// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

// uncomment this if you want proxies to be passable as inout and out parameters
// #define CXXSWIZZLE_VECTOR_INOUT_WRAPPER_ENABLED

#include <type_traits>
#include <swizzle/glsl/scalar_support.h>
#include <cstdint>

using batch_float_t = swizzle::detail::simd_float<>;
using batch_int_t = swizzle::detail::simd_int32<>;
using batch_uint_t = swizzle::detail::simd_uint32<>;
using batch_bool_t = swizzle::detail::simd_bool<>;

namespace swizzle
{
    namespace detail
    {
        template <> struct get_vector_type_impl<float> : get_vector_type_impl<batch_float_t> {};
        template <> struct get_vector_type_impl<double> : get_vector_type_impl<batch_float_t> {};
        template <> struct get_vector_type_impl<int> : get_vector_type_impl<batch_int_t> {};
        template <> struct get_vector_type_impl<uint32_t> : get_vector_type_impl<batch_uint_t> {};
    }
}


//
//typedef float batch_float_t;
//typedef float raw_batch_float_t;
//typedef uint32_t raw_batch_uint32_t;
//using batch_int_t = int32_t;
//using batch_uint_t = uint32_t;
//using batch_bool_t = bool;
//
