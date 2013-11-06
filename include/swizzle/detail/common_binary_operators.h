// CxxSwizzle
// Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

namespace swizzle
{
    namespace detail
    {
        //! Inspired by boost/operators, this class defines arithmetic operators for a vector
        //! and a scalar as friend inline functions (only accessible with ADL). The reason for that,
        //! contrary to having global template operators is there's less typing and types decaying
        //! to a vector/scalar (proxies!) can use these operators too.
        template <class VectorType, class ScalarType>
        struct common_binary_operators
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