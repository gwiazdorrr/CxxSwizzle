// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

// VC need to come first or else VC is going to complain.
#include <Vc/vector.h>
#include <swizzle/glsl/simd_support_vc.h>
// need to include scalars as well because we don't need literals
// to use simd (like sin(1))
#include <swizzle/glsl/scalar_support.h>

typedef swizzle::glsl::vc_float<> float_type;
typedef float_type::internal_type raw_float_type;
typedef Vc::uint_v uint_type;

//! By default Vc's masks (result of comparisons) decay to bools but
//! are not implicitly constructible; hence defining bool_type as bool
//! is the safest bet here.
typedef bool bool_type;

static_assert(static_cast<size_t>(raw_float_type::Size) == static_cast<size_t>(uint_type::Size), "Both float and uint types need to have same number of entries");
const size_t scalar_count = raw_float_type::Size;
const size_t float_entries_align = Vc::VectorAlignment;
const size_t uint_entries_align = Vc::VectorAlignment;

template <typename T>
inline void store_aligned(const Vc::Vector<T>& value, T* target)
{
    value.store(target, Vc::Aligned);
}

template <typename T>
inline void load_aligned(Vc::Vector<T>& value, const T* data)
{
    value.load(data, Vc::Aligned);
}

