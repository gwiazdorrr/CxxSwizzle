// CxxSwizzle
// Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>

#pragma once

#include <type_traits>
#include <swizzle/glsl/scalar_support.h>

typedef float float_type;
typedef unsigned uint_type;
typedef bool bool_type;

const size_t scalar_count = 1;
const size_t float_entries_align = std::alignment_of<float>::value;
const size_t uint_entries_align = std::alignment_of<unsigned>::value;

template <typename T>
inline void store_aligned(T&& value, typename std::remove_reference<T>::type* target)
{
    *target = std::forward<T>(value);
}

template <typename T>
inline void load_aligned(T& value, const T* data)
{
    value = *data;
}