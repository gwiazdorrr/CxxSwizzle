// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include "static_functors.h"

namespace swizzle
{
    namespace detail
    {
        //! Inspired by boost/operators, this class defines arithmetic operators for a vector
        //! and a scalar as friend inline functions (only accessible with ADL). The reason for that,
        //! contrary to having global template operators is there's less typing and types decaying
        //! to a vector/scalar (proxies!) can use these operators too.
        //! Note that because VC++ is unable to align SIMD types when passing by value,
        //! functions defined here use C++03 pass-by-reference style.
        template <typename VectorType, typename ScalarType, typename VectorArgType = const VectorType&, typename ScalarArgType = const ScalarType&>
        struct common_binary_operators
        {
            typedef VectorArgType VectorArgType;
            typedef ScalarArgType ScalarArgType;

            friend VectorType operator+(VectorArgType v, ScalarArgType s)
            {
                return VectorType(v) += s;
            }
            friend VectorType operator+(ScalarArgType s, VectorArgType v)
            {
                return v + s;
            }
            friend VectorType operator+(VectorArgType v1, VectorArgType v2)
            {
                return VectorType(v1) += v2;
            }

            friend VectorType operator*(VectorArgType v, ScalarArgType s)
            {
                return VectorType(v) *= s;
            }
            friend VectorType operator*(ScalarArgType s, VectorArgType v)
            {
                return v * s;
            }
            friend VectorType operator*(VectorArgType v1, VectorArgType v2)
            {
                return VectorType(v1) *= v2;
            }

            friend VectorType operator-(VectorArgType v, ScalarArgType s)
            {
                return VectorType(v) -= s;
            }
            friend VectorType operator-(ScalarArgType s, VectorArgType v)
            {
                return VectorType(s) -= v;
            }
            friend VectorType operator-(VectorArgType v1, VectorArgType v2)
            {
                return VectorType(v1) -= v2;
            }

            friend VectorType operator/(VectorArgType v, ScalarArgType s)
            {
                return VectorType(v) /= s;
            }
            friend VectorType operator/(ScalarArgType s, VectorArgType v)
            {
                return VectorType(s) /= v;
            }
            inline friend VectorType operator/(VectorArgType v1, VectorArgType v2)
            {
                return VectorType(v1) /= v2;
            }
        };
    }
}