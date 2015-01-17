// CxxSwizzle
// Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <cmath>

namespace swizzle
{
    namespace glsl
    {
        namespace naive
        {
            inline float rsqrt(float x)
            {
                return std::pow(x, -0.5f);
            }

            inline float sign(float x)
            {
                return (0 < x) - (x < 0);
            }

            inline double rsqrt(double x)
            {
                return std::pow(x, -0.5);
            }

            inline double sign(double x)
            {
                return (0 < x) - (x < 0);
            }
        }
    }
}