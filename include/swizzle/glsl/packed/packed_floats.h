#pragma once

#include <immintrin.h>
#include <xmmintrin.h>

namespace swizzle
{
    namespace glsl
    {
        namespace packed
        {
            class floats
            {
                __m256 data;

                floats(__m256 data) : data(data) 
                {}



            public:

                floats() {}

                floats(float f) : data(_mm256_set1_ps(f))
                {}

                // arithmetic operators

                floats& operator+=(const floats& other)
                {
                    return *this = *this + other;
                }

                inline friend floats operator+(const floats& a, const floats& b)
                {
                    return _mm256_add_ps(a.data, b.data);
                }

                floats& operator-=(const floats& other)
                {
                    return *this = *this - other;
                }

                inline friend floats operator-(const floats& a, const floats& b)
                {
                    return _mm256_sub_ps(a.data, b.data);
                }

                floats& operator*=(const floats& other)
                {
                    return *this = *this * other;
                }

                inline friend floats operator*(const floats& a, const floats& b)
                {
                    return _mm256_mul_ps(a.data, b.data);
                }

                floats& operator/=(const floats& other)
                {
                    return *this = *this / other;
                }

                inline friend floats operator/(const floats& a, const floats& b)
                {
                    return _mm256_div_ps(a.data, b.data);
                }

                // comparisons

                static float compare(const floats& a, const floats& b)
                {
                    auto dota = _mm256_dp_ps(a.data, a.data, 0xf1);
                    auto dotb = _mm256_dp_ps(b.data, b.data, 0xf1);
                    auto partialDif = _mm256_sub_ps(dota, dotb);
                    // this hurts
                    return partialDif.m256_f32[0] + partialDif.m256_f32[4];
                }

                inline friend bool operator<(const floats& a, const floats& b)
                {
                    return compare(a, b) < 0;
                }

                inline friend bool operator<=(const floats& a, const floats& b)
                {
                    return compare(a, b) <= 0;
                }

                inline friend bool operator>(const floats& a, const floats& b)
                {
                    return compare(a, b) > 0;
                }

                inline friend bool operator>=(const floats& a, const floats& b)
                {
                    return compare(a, b) <= 0;
                }

                inline friend bool operator==(const floats& a, const floats& b)
                {
                    return compare(a, b) == 0;
                }

                inline friend bool operator!=(const floats& a, const floats& b)
                {
                    return compare(a, b) != 0;
                }


                // math functions

                inline friend floats sqrt(const floats& a)
                {
                    return _mm256_sqrt_ps(a.data);
                }

                inline friend floats rsqrt(const floats& a)
                {
                    return _mm256_rsqrt_ps(a.data);
                }


            };

            

        }
    }
}

    