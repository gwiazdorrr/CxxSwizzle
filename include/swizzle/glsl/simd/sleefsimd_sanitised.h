#pragma once

/**
Basically a sanitised version of sleefsimd.h. It compiles in visual c++ and defines symbol in less
conflict-prone way, where possible.
*/



#ifdef CXXSWIZZLE_SIMD_ENABLED

#include <stdint.h>

#ifdef CXXSWIZZLE_ENABLE_SSE2

#include <immintrin.h>

#define SLEEF_VECTLENDP 2
#define SLEEF_VECTLENSP 4

typedef __m128d vdouble;
typedef __m128i vint;
typedef __m128 vfloat;

#endif

#if defined(CXXSWIZZLE_ENABLE_AVX) || defined(CXXSWIZZLE_ENABLE_FMA4)

#include <immintrin.h>

#define SLEEF_VECTLENDP 4
#define SLEEF_VECTLENSP 8

typedef __m256d vdouble;
typedef __m128i vint;
typedef __m256 vfloat;

#endif

#ifdef CXXSWIZZLE_ENABLE_AVX2

#include <immintrin.h>

#define SLEEF_VECTLENDP 4
#define SLEEF_VECTLENSP 8

typedef __m256d vdouble;
typedef __m128i vint;
typedef __m256 vfloat;

#endif


/* double precision */

typedef struct {
    vdouble x, y;
} vdouble2;

vdouble xsin(vdouble d);
vdouble xcos(vdouble d);
vdouble2 xsincos(vdouble d);
vdouble xtan(vdouble d);
vdouble xasin(vdouble s);
vdouble xacos(vdouble s);
vdouble xatan(vdouble s);
vdouble xatan2(vdouble y, vdouble x);
vdouble xlog(vdouble d);
vdouble xexp(vdouble d);
vdouble xpow(vdouble x, vdouble y);

vdouble xsinh(vdouble d);
vdouble xcosh(vdouble d);
vdouble xtanh(vdouble d);
vdouble xasinh(vdouble s);
vdouble xacosh(vdouble s);
vdouble xatanh(vdouble s);

vdouble xcbrt(vdouble d);

vdouble xexp2(vdouble a);
vdouble xexp10(vdouble a);
vdouble xexpm1(vdouble a);
vdouble xlog10(vdouble a);
vdouble xlog1p(vdouble a);

vdouble xsin_u1(vdouble d);
vdouble xcos_u1(vdouble d);
vdouble2 xsincos_u1(vdouble d);
vdouble xtan_u1(vdouble d);
vdouble xasin_u1(vdouble s);
vdouble xacos_u1(vdouble s);
vdouble xatan_u1(vdouble s);
vdouble xatan2_u1(vdouble y, vdouble x);
vdouble xlog_u1(vdouble d);
vdouble xcbrt_u1(vdouble d);

/* single precision */

typedef struct {
    vfloat x, y;
} vfloat2;

vfloat xsinf(vfloat d);
vfloat xcosf(vfloat d);
vfloat2 xsincosf(vfloat d);
vfloat xtanf(vfloat d);
vfloat xasinf(vfloat s);
vfloat xacosf(vfloat s);
vfloat xatanf(vfloat s);
vfloat xatan2f(vfloat y, vfloat x);
vfloat xlogf(vfloat d);
vfloat xexpf(vfloat d);
vfloat xcbrtf(vfloat s);
vfloat xsqrtf(vfloat s);

vfloat xpowf(vfloat x, vfloat y);
vfloat xsinhf(vfloat x);
vfloat xcoshf(vfloat x);
vfloat xtanhf(vfloat x);
vfloat xasinhf(vfloat x);
vfloat xacoshf(vfloat x);
vfloat xatanhf(vfloat x);
vfloat xexp2f(vfloat a);
vfloat xexp10f(vfloat a);
vfloat xexpm1f(vfloat a);
vfloat xlog10f(vfloat a);
vfloat xlog1pf(vfloat a);

vfloat xsinf_u1(vfloat d);
vfloat xcosf_u1(vfloat d);
vfloat2 xsincosf_u1(vfloat d);
vfloat xtanf_u1(vfloat d);
vfloat xasinf_u1(vfloat s);
vfloat xacosf_u1(vfloat s);
vfloat xatanf_u1(vfloat s);
vfloat xatan2f_u1(vfloat y, vfloat x);
vfloat xlogf_u1(vfloat d);
vfloat xcbrtf_u1(vfloat s);

#endif