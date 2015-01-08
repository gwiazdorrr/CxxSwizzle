#pragma once

#define CXXSWIZZLE_ENABLE_SSE2

#if defined(CXXSWIZZLE_ENABLE_SSE2) || defined(CXXSWIZZLE_ENABLE_AVX) || defined(CXXSWIZZLE_ENABLE_FMA4) || defined(CXXSWIZZLE_ENABLE_AVX2)
#define CXXSWIZZLE_SIMD_ENABLED
#endif

#ifdef CXXSWIZZLE_SIMD_ENABLED
extern "C"
{
#include "sleefsimd_sanitised.h"
}

namespace swizzle
{
    namespace glsl
    {
        namespace simd
        {
            inline vdouble sin(vdouble v) { return ::xsin(v); }


            inline vfloat sin(vfloat v) { return ::xsinf(v); }

        }
    }
}

#endif