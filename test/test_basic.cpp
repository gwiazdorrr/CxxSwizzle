// CxxSwizzle
// Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>

#include <gtest/gtest.h>
#include <array>
#include <limits>
#include "setup.h"


template <class TVec, class TScalar>
void test_operators(TVec a, TScalar b)
{
    size_t index = 0;

    auto negPredicate = [=](typename TVec::scalar_type x, typename TVec::scalar_type y) -> bool { return are_close(x, -y); };
    auto addPredicate = [=](typename TVec::scalar_type x, typename TVec::scalar_type y) -> bool { return are_close(x, y + b); };
    auto subPredicate = [=](typename TVec::scalar_type x, typename TVec::scalar_type y) -> bool { return are_close(x, y - b); };
    auto mulPredicate = [=](typename TVec::scalar_type x, typename TVec::scalar_type y) -> bool { return are_close(x, y * b); };
    auto divPredicate = [=](typename TVec::scalar_type x, typename TVec::scalar_type y) -> bool { return are_close(x, y / b); };

    

    EXPECT_TRUE( are_equal(-a, a, negPredicate) );

    EXPECT_TRUE( are_equal<TVec>(a + b, a, addPredicate) );
    EXPECT_TRUE( are_equal<TVec>(b + a, a, addPredicate) );
    EXPECT_TRUE( are_equal<TVec>(TVec(a) += b, a, addPredicate) );

    EXPECT_TRUE( are_equal<TVec>(a - b, a, subPredicate) );
    EXPECT_TRUE( are_equal<TVec>(-(b - a), a, subPredicate) );
    EXPECT_TRUE( are_equal<TVec>(TVec(a) -= b, a, subPredicate) );

    EXPECT_TRUE( are_equal<TVec>(a * b, a, mulPredicate) );
    EXPECT_TRUE( are_equal<TVec>(b * a, a, mulPredicate) );
    EXPECT_TRUE( are_equal<TVec>(TVec(a) *= b, a, mulPredicate) );

    EXPECT_TRUE( are_equal<TVec>(a / b, a, divPredicate) );
    EXPECT_TRUE( are_equal<TVec>(1 / (b / a), a, divPredicate) );
    EXPECT_TRUE( are_equal<TVec>(TVec(a) /= b, a, divPredicate) );
}

template <class TVec>
void test_oprators(TVec a)
{
    test_operators(a, double(1));
    test_operators(a, int(1));
    test_operators(a, float(1));
}

TEST(Construction, vec1_basic)
{
    // default
    vec1 a;
    EXPECT_EQ(a.x, 0.0f);
    // with a value
    vec1 b(666);
    EXPECT_EQ(b.x, 666.0f);
    // copy
    vec1 c(b);
    EXPECT_EQ(b.x, c.x);
    // from proxy
    vec1 d(c.x);
    EXPECT_EQ(c.x, d.x);
}

TEST(Construction, vec2_basic)
{
    EXPECT_TRUE( are_all_equal( vec2(), 0) );
    EXPECT_TRUE( !are_all_equal( vec2(), 666) );
    EXPECT_TRUE( are_all_equal( vec2(666), 666) );

    // with a value
    vec2 b(1, 2);
    EXPECT_TRUE(b.x == 1);
    EXPECT_TRUE(b.y == 2);
    EXPECT_TRUE( !are_all_equal(b, 666) );

    // copy
    vec2 c(b);
    EXPECT_TRUE(are_equal(b, c));
    // from proxy (reverse)
    vec2 d(c.yx);
    EXPECT_TRUE(are_equal_reverse(c, d));
    c.yx = c;
}

TEST(Construction, vec3_basic)
{
    EXPECT_TRUE( are_all_equal( vec3(), 0) );
    EXPECT_TRUE( !are_all_equal( vec3(), 666) );
    EXPECT_TRUE( are_all_equal( vec3(666), 666) );

    // with a value
    vec3 b(1, 2, 3);
    EXPECT_TRUE(b.x == 1);
    EXPECT_TRUE(b.y == 2);
    EXPECT_TRUE(b.z == 3);
    EXPECT_TRUE( !are_all_equal(b, 666) );

    // copy
    vec3 c(b);
    EXPECT_TRUE(are_equal(b, c));
    // from proxy (reverse)
    vec3 d(c.zyx);
    EXPECT_TRUE(are_equal_reverse(c, d));
}

TEST(Construction, vec4_basic)
{
    EXPECT_TRUE( are_all_equal( vec4(), 0) );
    EXPECT_TRUE( !are_all_equal( vec4(), 666) );
    EXPECT_TRUE( are_all_equal( vec4(666), 666) );

    // with a value
    vec4 b(1, 2, 3, 4);
    EXPECT_TRUE( (b.x == 1 && b.y == 2 && b.z == 3 && b.w == 4) );
    EXPECT_TRUE( !are_all_equal(b, 666) );


    // copy
    vec4 c(b);
    EXPECT_TRUE(are_equal(b, c));
    // from proxy (reverse)
    vec4 d(c.wzyx);
    EXPECT_TRUE(are_equal_reverse(c, d));
}

TEST(Construction, vec_proxy_test)
{
    vec4 v1(1, 2, 3, 4);

    // check if values make sense

    EXPECT_TRUE(  are_equal<vec1>(vec1(1), vec1(v1.x)) );
    EXPECT_TRUE(  are_equal<vec1>(vec1(4), vec1(v1.w)) );
    EXPECT_TRUE( !are_equal<vec1>(vec1(1), vec1(v1.w)) );

    EXPECT_TRUE(  are_equal<vec2>(vec2(1, 2), v1.xy) );
    EXPECT_TRUE(  are_equal<vec2>(vec2(4, 3), v1.wz) );
    EXPECT_TRUE(  are_equal<vec2>(vec2(1, 4), v1.xw) );
    EXPECT_TRUE( !are_equal<vec2>(vec2(1, 2), v1.xw) );

    EXPECT_TRUE(  are_equal<vec3>(vec3(1, 2, 3), v1.xyz) );
    EXPECT_TRUE(  are_equal<vec3>(vec3(4, 3, 2), v1.wzy) );
    EXPECT_TRUE(  are_equal<vec3>(vec3(1, 4, 4), v1.xww) );
    EXPECT_TRUE( !are_equal<vec3>(vec3(1, 2, 3), v1.xww) );

    EXPECT_TRUE(  are_equal<vec4>(vec4(1, 2, 3, 4), v1.xyzw) );
    EXPECT_TRUE(  are_equal<vec4>(vec4(4, 3, 2, 1), v1.wzyx) );
    EXPECT_TRUE(  are_equal<vec4>(vec4(3, 2, 2, 4), v1.zyyw) );
}

TEST(Construction, vec_test_operators)
{
    test_oprators( vec1(0) );
    test_oprators( vec2(1, 2) );
    test_oprators( vec3(1, 2, 3) );
    test_oprators( vec4(1, 2, 3, 4) );
}

TEST(Construction, vec1_test_construct)
{
    // if it compiles then great
    vec1 v1(1);
    vec2 v2(2, 3);
    vec3 v3(4, 5, 6);
    vec4 v4(7, 8, 9, 10);

    EXPECT_TRUE( are_equal(vec1(v1.x), v1)) ;
    //BOOST_TEST( are_equal(vec1(v2.y), vec1(v2.yxyx)) );
    EXPECT_TRUE( are_equal(vec1(v3.x), vec1(v3)) );
    //BOOST_TEST( are_equal(vec1(v4.w), vec1(v4.wwww)) );
}

TEST(Construction, vec2_test_construct)
{
    // if it compiles then great
    vec1 v1(1);
    vec2 v2(2, 3);
    vec3 v3(4, 5, 6);
    vec4 v4(7, 8, 9, 10);

    EXPECT_TRUE( are_equal(vec2(v1.x, v1.x), vec2(v1.xx)) );
    EXPECT_TRUE( are_equal(vec2(v2.x, v2.y), vec2(v2.x, v2.yx)) );
    //BOOST_TEST( are_equal(vec2(v3.z, v3.y), vec2(v3.zyzz)) );
    EXPECT_TRUE( are_equal(vec2(v4.w, v4.w), vec2(v4.w, v4.wwxx)) );
}

TEST(Construction, vec3_test_construct)
{
    // if it compiles then great
    vec1 v1(1);
    vec2 v2(2, 3);
    vec3 v3(4, 5, 6);
    vec4 v4(7, 8, 9, 10);

    //BOOST_TEST( are_equal(vec3(v1.x, v1.x, v1.x), vec3(v1.xxxx)) );
    EXPECT_TRUE( are_equal(vec3(v2.x, v2.y, v2.x), vec3(v2.x, v2.yx)) );
    EXPECT_TRUE( are_equal(vec3(v3.x, v3.y, v3.z), vec3(v3)) );
    EXPECT_TRUE( are_equal(vec3(v4.w, v4.w, v4.w), vec3(v4.w, v4.wwxx)) );
}

TEST(Construction, vec4_test_construct)
{
    // if it compiles then great
    vec1 v1(1);
    vec2 v2(2, 3);
    vec3 v3(4, 5, 6);
    vec4 v4(7, 8, 9, 10);

    EXPECT_TRUE( are_equal(vec4(v1.x, v1.x, v1.x, v1.x), vec4(v1.xxxx)) );
    EXPECT_TRUE( are_equal(vec4(v2.x, v2.y, v2.x, v2.x), vec4(v2.x, v2.y, v2.x, v2.xyxy)) );
    EXPECT_TRUE( are_equal(vec4(v3.x, v3.y, v3.z, v3.z), vec4(v3, v3.z)) );
    EXPECT_TRUE( are_equal(vec4(v4.w, v4.w, v4.w, v4.x), vec4(v4.w, v4.wwxx)) );
}