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
            typedef VectorArgType vector_arg_type;
            typedef ScalarArgType scalar_arg_type;

            friend VectorType operator+(vector_arg_type v, scalar_arg_type s)
            {
                VectorType result(v);
                return result += s;
            }
            friend VectorType operator+(scalar_arg_type s, vector_arg_type v)
            {
                return v + s;
            }
            friend VectorType operator+(vector_arg_type v1, vector_arg_type v2)
            {
                VectorType result(v1);
                return result += v2;
            }

            friend VectorType operator*(vector_arg_type v, scalar_arg_type s)
            {
                VectorType result(v);
                return result *= s;
            }
            friend VectorType operator*(scalar_arg_type s, vector_arg_type v)
            {
                return v * s;
            }
            friend VectorType operator*(vector_arg_type v1, vector_arg_type v2)
            {
                VectorType result(v1);
                return result *= v2;
            }

            friend VectorType operator-(vector_arg_type v, scalar_arg_type s)
            {
                VectorType result(v);
                return result -= s;
            }
            friend VectorType operator-(scalar_arg_type s, vector_arg_type v)
            {
                VectorType result(s);
                return result -= v;
            }
            friend VectorType operator-(vector_arg_type v1, vector_arg_type v2)
            {
                VectorType result(v1);
                return result -= v2;
            }

            friend VectorType operator/(vector_arg_type v, scalar_arg_type s)
            {
                VectorType result(v);
                return result /= s;
            }
            friend VectorType operator/(scalar_arg_type s, vector_arg_type v)
            {
                VectorType result(s);
                return result /= v;
            }
            inline friend VectorType operator/(vector_arg_type v1, vector_arg_type v2)
            {
                VectorType result(v1);
                return result /= v2;
            }
        };
    }
}