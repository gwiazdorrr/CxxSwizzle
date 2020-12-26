// CxxSwizzle
// Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>

#define CXXSWIZZLE_VECTOR_INOUT_WRAPPER_ENABLED
#include <gtest/gtest.h>
#include <array>
#include <limits>
#include "setup.h"

namespace
{
    void foo(swizzle::inout_wrapper<vec2> a)
    {}

    void foo(swizzle::inout_wrapper<vec3> a)
    {}

    void foo(swizzle::inout_wrapper<vec4> a)
    {}
}


TEST(Proxies, passing)
{
    vec4 v;
    foo(v.xz);
    foo(v.xzy);
    foo(v.xzyw);
}