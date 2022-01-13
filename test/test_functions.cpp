// CxxSwizzle (c) 2013-2021 Piotr Gwiazdowski

#include <gtest/gtest.h>

#include "setup.h"

#define VAR21(f, a, b)      f(a);f(b);

#define VAR31(f, a, b, c)   f(a);f(b);f(c);

#define VAR22(f, a, b)      f(a,a);f(a,b);f(b,a);f(b,b);

#define VAR32(f, a, b, c)   f(a,a);f(a,b);f(a,c);\
                            f(b,a);f(b,b);f(b,c);\
                            f(c,a);f(c,b);f(c,c);

#define VAR33(f, a, b, c)   f(a,a,a);f(a,a,b);f(a,a,c);\
                            f(a,b,a);f(a,b,b);f(a,b,c);\
                            f(a,c,a);f(a,c,b);f(a,c,c);\
                            f(b,a,a);f(b,a,b);f(b,a,c);\
                            f(b,b,a);f(b,b,b);f(b,b,c);\
                            f(b,c,a);f(b,c,b);f(b,c,c);\
                            f(c,a,a);f(c,a,b);f(c,a,c);\
                            f(c,b,a);f(c,b,b);f(c,b,c);\
                            f(c,c,a);f(c,c,b);f(c,c,c);

#define F_IMPL1(func, ret, a1) \
func(v1);\
func(v2);\
func(v3);\
func(v4);\
func(dd);\
func(ff);\
func(v4.x);\
func(v4.xy);\
func(v4.xyz);\
func(v4.xyzw);\
func(v4.xx);

#define F_IMPL2_NO_DOUBLE(func, ret, a1, a2) \
VAR22(func,v1,ff);\
func(v1,v1);\
func(v2,v2);\
func(v3,v3);\
func(v4,v4);

#define F_IMPL2(func, ret, a1, a2) \
VAR32(func,dd,v1,ff);\
VAR32(func,v2,v3.xy,v4.xx);\
func(v1,v1);\
func(v2,v2);\
func(v3,v3);\
func(v4,v4);

#define F_IMPL3(func, ret, a1, a2, a3) \
VAR33(func, dd, v1, ff); \
VAR33(func,v2,v3.xy,v4.xx);\
func(v1, v1, v1); \
func(v2, v2, v2); \
func(v3, v3, v3); \
func(v4, v4, v4);

#define F_IMPL2F1(func, ret, a1, a2) \
func(ff,dd);func(dd,dd);\
func(ff,ff);func(dd,ff);\
func(ff,v1);func(dd,v1);\
func(ff,v2);func(dd,v2);\
func(ff,v3);func(dd,v3);\
func(ff,v4);func(dd,v4);

#define F_IMPL2F2(func, ret, a1, a2) \
func(dd,ff);func(dd,dd);\
func(ff,ff);func(ff,dd);\
func(v1,ff);func(v1,dd);\
func(v2,ff);func(v2,dd);\
func(v3,ff);func(v3,dd);\
func(v4,ff);func(v4,dd);

#define F_IMPL3F3(func, ret, a1, a2, a3) \
func(dd,dd,ff);func(dd,dd,dd);\
func(ff,ff,ff);func(ff,ff,dd);\
func(v1,v1,ff);func(v1,v1,dd);\
func(v2,v2,ff);func(v2,v2,dd);\
func(v3,v3,ff);func(v3,v3,dd);\
func(v4,v4,ff);func(v4,v4,dd);

#define F_IMPL3F23(func, ret, a1, a2, a3) \
func(dd,ff,ff);func(dd,ff,dd);func(dd,dd,dd);func(dd,dd,ff);\
func(ff,ff,ff);func(ff,ff,dd);func(ff,dd,dd);func(ff,dd,ff);\
func(v1,ff,ff);func(v1,ff,dd);func(v1,dd,dd);func(v1,dd,ff);\
func(v2,ff,ff);func(v2,ff,dd);func(v2,dd,dd);func(v2,dd,ff);\
func(v3,ff,ff);func(v3,ff,dd);func(v3,dd,dd);func(v3,dd,ff);\
func(v4,ff,ff);func(v4,ff,dd);func(v4,dd,dd);func(v4,dd,ff);

#define F_IMPL3F12(func, ret, a1, a2, a3) \
func(ff,ff,dd);func(ff,dd,dd);func(dd,dd,dd);func(dd,ff,dd);\
func(ff,ff,ff);func(ff,dd,ff);func(dd,dd,ff);func(dd,ff,ff);\
func(ff,ff,v1);func(ff,dd,v1);func(dd,dd,v1);func(dd,ff,v1);\
func(ff,ff,v2);func(ff,dd,v2);func(dd,dd,v2);func(dd,ff,v2);\
func(ff,ff,v3);func(ff,dd,v3);func(dd,dd,v3);func(dd,ff,v3);\
func(ff,ff,v4);func(ff,dd,v4);func(dd,dd,v4);func(dd,ff,v4);

float ff;
double dd;
vec1 v1;
vec2 v2;
vec3 v3;
vec4 v4;

TEST(Functions, Par_8_1)
{

    F_IMPL1(radians, genType_ret, degrees)
    F_IMPL1(degrees, genType_ret, radians)
    F_IMPL1(cos, genType_ret, angle)
    F_IMPL1(tan, genType_ret, angle)
    F_IMPL1(sin, genType_ret, angle)
    F_IMPL1(asin, genType_ret, x)
    F_IMPL1(acos, genType_ret, x)
    F_IMPL1(atan, genType_ret, y_over_x)
    F_IMPL2(atan, genType_ret, y, x)
    F_IMPL1(sinh, genType_ret, x)
    F_IMPL1(cosh, genType_ret, x)
    F_IMPL1(tanh, genType_ret, x)
    F_IMPL1(asinh, genType_ret, x)
    F_IMPL1(acosh, genType_ret, x)
    F_IMPL1(atanh, genType_ret, x)
}

TEST(Functions, Par_8_2)
{
    F_IMPL2(pow, genType_ret, x, y)
    F_IMPL1(exp, genType_ret, x)
    F_IMPL1(log, genType_ret, x)
    F_IMPL1(exp2, genType_ret, x)
    F_IMPL1(log2, genType_ret, x)
    F_IMPL1(sqrt, genType_ret, x)
    F_IMPL1(inversesqrt, genType_ret, x)
}

TEST(Functions, Par_8_3)
{
    F_IMPL1(abs, genType_ret, x)
    F_IMPL1(sign, genType_ret, x)
    F_IMPL1(floor, genType_ret, x)
    F_IMPL1(trunc, genType_ret, x)
    F_IMPL1(round, genType_ret, x)
    F_IMPL1(roundEven, genType_ret, x)
    F_IMPL1(ceil, genType_ret, x)
    F_IMPL1(fract, genType_ret, x)
    F_IMPL2(mod, genType_ret, x, y)
    F_IMPL2_NO_DOUBLE(modf, genType_ret, x, i)

    F_IMPL2(min, genType_ret, x, y)
    F_IMPL2(max, genType_ret, x, y)
    F_IMPL3(clamp, genType_ret, x, minVal, maxVal)
    F_IMPL3(mix, genType_ret, x, y, a)
    F_IMPL2(step, genType_ret, edge, x)
    F_IMPL3(smoothstep, genType_ret, edge0, edge1, x)
    F_IMPL3(mix, genType_ret, x, y, a)
    // TODO: cmath gets called instead for some reason
    //F_IMPL1(isnan, genBType_ret, x)
    //F_IMPL1(isinf, genBType_ret, x)

    F_IMPL2F2(mod, genType_ret, x, y)
    F_IMPL2F2(min, genType_ret, x, y)
    F_IMPL2F2(max, genType_ret, x, y)
    F_IMPL3F23(clamp, genType_ret, x, minVal, maxVal)
    F_IMPL3F3(mix, genType_ret, x, y, a)
    F_IMPL2F1(step, genType_ret, edge, x)
    F_IMPL3F12(smoothstep, genType_ret, edge0, edge1, x)
}

TEST(Functions, Par_8_4)
{
    F_IMPL1(length, float_arg, x)
    F_IMPL2(distance, float_arg, p0, p1)
    F_IMPL2(dot, float_arg, x, y)

    cross(v3, v3);

    F_IMPL1(normalize, genType_ret, x)
    F_IMPL3(faceforward, genType_ret, N, I, Nref)
    F_IMPL2(reflect, genType_ret, N, I)
    F_IMPL3F3(refract, genType_ret, N, I, Nref)
}

TEST(Functions, Par_8_6)
{
    F_IMPL2(lessThan, bvec, x, y)
    F_IMPL2(lessThan, bvec, x, y)
    F_IMPL2(lessThan, bvec, x, y)
    F_IMPL2(lessThanEqual, bvec, x, y)
    F_IMPL2(lessThanEqual, bvec, x, y)
    F_IMPL2(lessThanEqual, bvec, x, y)
    F_IMPL2(greaterThan, bvec, x, y)
    F_IMPL2(greaterThan, bvec, x, y)
    F_IMPL2(greaterThan, bvec, x, y)
    F_IMPL2(greaterThanEqual, bvec, x, y)
    F_IMPL2(greaterThanEqual, bvec, x, y)
    F_IMPL2(greaterThanEqual, bvec, x, y)
    F_IMPL2(equal, bvec, x, y)
    F_IMPL2(equal, bvec, x, y)
    F_IMPL2(equal, bvec, x, y)
    F_IMPL2(notEqual, bvec, x, y)
    F_IMPL2(notEqual, bvec, x, y)
    F_IMPL2(notEqual, bvec, x, y)
    // bvec only
    //F_IMPL1(any, bool, x)
    //F_IMPL1(all, bool, x)
    //F_IMPL1(not, bool, x)
}