//  CxxSwizzle
//  Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr.os@gmail.com>
#pragma once

#include <utility>
#include <swizzle/detail/vector_traits.h>

namespace swizzle
{
    namespace detail
    {
        template <class VectorType, class ScalarType>
        struct binary_operators
        {
            friend VectorType operator+(VectorType v, ScalarType s)
            {
                return v += s;
            }
            friend VectorType operator+(ScalarType s, VectorType v)
            {
                return v += s;
            }
            friend VectorType operator+(VectorType v1, const VectorType& v2)
            {
                return v1 += v2;
            }

            friend VectorType operator*(VectorType v, ScalarType s)
            {
                return v *= s;
            }
            friend VectorType operator*(ScalarType s, VectorType v)
            {
                return v *= s;
            }
            friend VectorType operator*(VectorType v1, const VectorType& v2)
            {
                return v1 *= v2;
            }

            friend VectorType operator-(VectorType v, ScalarType s)
            {
                return v -= s;
            }
            friend VectorType operator-(ScalarType s, const VectorType& v)
            {
                return VectorType(s) -= v;
            }
            friend VectorType operator-(VectorType v1, const VectorType& v2)
            {
                return v1 -= v2;
            }

            friend VectorType operator/(VectorType v, ScalarType s)
            {
                return v /= s;
            }
            friend VectorType operator/(ScalarType s, const VectorType& v)
            {
                return VectorType(s) /= v;
            }
            friend VectorType operator/(VectorType v1, const VectorType& v2)
            {
                return v1 /= v2;
            }
        };
    }
}