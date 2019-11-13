// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

// uncomment this if you want proxies to be passable as inout and out parameters
// #define CXXSWIZZLE_inout_wrapper_ENABLED

#include <swizzle/setup_scalar.hpp>


//namespace swizzle
//{
//    namespace detail
//    {
//        template <> struct get_vector_type_impl<float> : default_vector_type_impl<batch_float_t> {};
//        template <> struct get_vector_type_impl<double> : default_vector_type_impl<batch_float_t> {};
//        template <> struct get_vector_type_impl<int> : default_vector_type_impl<batch_int_t> {};
//        template <> struct get_vector_type_impl<uint32_t> : default_vector_type_impl<batch_uint_t> {};
//        template <> struct get_vector_type_impl<batch_bool_t> : default_vector_type_impl<batch_bool_t>
//        {};
//    }
//}


//
//typedef float batch_float_t;
//typedef float raw_batch_float_t;
//typedef uint32_t raw_batch_uint32_t;
//using batch_int_t = int32_t;
//using batch_uint_t = uint32_t;
//using batch_bool_t = bool;
//
