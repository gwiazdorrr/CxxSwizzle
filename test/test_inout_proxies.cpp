// CxxSwizzle
// Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>

#include <gtest/gtest.h>

#include "setup.h"

using namespace swizzle;

using inout_vec2 = swizzle::inout_wrapper<vec2>;

static void foo_v2(vec2) {}
static void foo_iv2(inout_vec2) {}

TEST(inout_proxy, basic)
{
    vec2 _backing_vec2;
    vec2 v2;
    vec4 v4;

    {
        inout_vec2 iv2(_backing_vec2);
        v2 = vec2(1, 0);
        iv2 = v2;
    }

    EXPECT_TRUE(are_equal(_backing_vec2, v2));


    {
        inout_vec2 iv2(_backing_vec2);
        iv2 = v2.yx;
    }

    EXPECT_EQ(_backing_vec2.x, v2.y);
    EXPECT_EQ(_backing_vec2.y, v2.x);

    {
        inout_vec2 iv2(_backing_vec2);
        v4.x = 10;
        v4.w = 66;
        iv2 = v4.wx;
    }

    EXPECT_EQ(_backing_vec2.x, v4.w);
    EXPECT_EQ(_backing_vec2.y, v4.x);



    {
        inout_vec2 iv2 = v2;
        foo_v2(v2);
        foo_v2(iv2);
        foo_iv2(v2);
        foo_iv2(iv2);
    }
}