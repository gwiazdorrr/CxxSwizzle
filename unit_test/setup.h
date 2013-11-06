// CxxSwizzle
// Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <swizzle/glsl/naive/vector.h>
#include <swizzle/glsl/naive/matrix.h>
#include <swizzle/glsl/vector_functions.h>

typedef swizzle::glsl::naive::vector< float, 1 > vec1;
typedef swizzle::glsl::naive::vector< float, 2 > vec2;
typedef swizzle::glsl::naive::vector< float, 3 > vec3;
typedef swizzle::glsl::naive::vector< float, 4 > vec4;

typedef swizzle::glsl::naive::vector< double, 2 > dvec2;
typedef swizzle::glsl::naive::vector< double, 3 > dvec3;
typedef swizzle::glsl::naive::vector< double, 4 > dvec4;

typedef swizzle::glsl::naive::vector< bool, 2 > bvec2;
typedef swizzle::glsl::naive::vector< bool, 3 > bvec3;
typedef swizzle::glsl::naive::vector< bool, 4 > bvec4;

typedef swizzle::glsl::naive::vector< int, 2 > ivec2;
typedef swizzle::glsl::naive::vector< int, 3 > ivec3;
typedef swizzle::glsl::naive::vector< int, 4 > ivec4;


typedef swizzle::glsl::naive::matrix< swizzle::glsl::naive::vector, float, 2, 2> mat2;
typedef swizzle::glsl::naive::matrix< swizzle::glsl::naive::vector, float, 3, 3> mat3;
typedef swizzle::glsl::naive::matrix< swizzle::glsl::naive::vector, float, 4, 4> mat4;

typedef mat2 mat2x2;
typedef mat3 mat3x3;
typedef mat4 mat4x4;

typedef swizzle::glsl::naive::matrix< swizzle::glsl::naive::vector, float, 3, 2> mat3x2;
typedef swizzle::glsl::naive::matrix< swizzle::glsl::naive::vector, float, 4, 2> mat4x2;
typedef swizzle::glsl::naive::matrix< swizzle::glsl::naive::vector, float, 2, 3> mat2x3;
typedef swizzle::glsl::naive::matrix< swizzle::glsl::naive::vector, float, 3, 4> mat3x4;

typedef swizzle::glsl::naive::matrix< swizzle::glsl::naive::vector, double, 2, 2> dmat2;
typedef swizzle::glsl::naive::matrix< swizzle::glsl::naive::vector, double, 3, 3> dmat3;
typedef swizzle::glsl::naive::matrix< swizzle::glsl::naive::vector, double, 4, 4> dmat4;

typedef swizzle::glsl::naive::matrix< swizzle::glsl::naive::vector, double, 2, 4> dmat2x4;


#include <array>
#include <limits>

inline bool are_close(float a, float b)
{
    static const float eps = std::numeric_limits<float>::epsilon();
    return (a-b) <= eps;
}

template <class TVec>
inline bool are_equal(const TVec& a, const TVec& b)
{
    return std::equal(a.begin(), a.end(), b.begin());
}

template <class TVec>
inline bool are_equal_reverse(const TVec& a, const TVec& b)
{
    typedef std::reverse_iterator< decltype(b.begin()) > reverse_iterator;
    return std::equal(a.begin(), a.end(), reverse_iterator(b.end()));
}

template <class TVec>
inline bool are_all_equal(const TVec& a, typename TVec::scalar_type v)
{
    return std::all_of(a.begin(), a.end(), [=](typename TVec::scalar_type x) -> bool { return x == v; } );
}

template <class TVec>
inline bool are_all_equal_list(const TVec& a, typename TVec::array_type &b)
{
    return std::equal(a.begin(), a.end(), std::begin(b));
}

template <class TVec, class Func>
inline bool are_all_of(const TVec& a, Func func)
{
    return std::all_of(a.begin(), a.end(), func);
}

template <class TVec, class Func>
inline bool are_equal(const TVec& a, const TVec& b, Func func)
{
    return std::equal(a.begin(), a.end(), b.begin(), func);
}