// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

// uncomment this if you want proxies to be passable as inout and out parameters
// #define CXXSWIZZLE_VECTOR_INOUT_WRAPPER_ENABLED

#include <type_traits>
#include <swizzle/glsl/scalar_support.h>
#include <cstdint>

typedef float batch_float_t;
typedef float raw_batch_float_t;
typedef uint32_t raw_batch_uint32_t;

const size_t batch_scalar_count = 1;
const size_t batch_float_align = std::alignment_of<raw_batch_float_t>::value;
const size_t batch_uint32_align = std::alignment_of<raw_batch_uint32_t>::value;

template <typename T>
inline void batch_store_aligned(T&& value, typename std::remove_reference<T>::type* target)
{
    *target = std::forward<T>(value);
}

template <typename T>
inline void batch_load_aligned(T& value, const T* data)
{
    value = *data;
}

template <typename To, typename From>
To batch_cast(From&& value)
{
    return static_cast<To>(value);
}

