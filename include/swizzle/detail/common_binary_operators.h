// CxxSwizzle
// Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
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
        template <class VectorType, class ScalarType>
        struct common_binary_operators
        {
            typedef const VectorType& vector_arg;
            typedef const ScalarType& scalar_arg;

            friend VectorType operator+(vector_arg v, scalar_arg s)
            {
                return static_foreach<functor_add>(VectorType(v), s);
            }
            friend VectorType operator+(scalar_arg s, vector_arg v)
            {
                return v + s;
            }
            friend VectorType operator+(vector_arg v1, vector_arg v2)
            {
                return static_foreach<functor_add>(VectorType(v1), v2);
            }

            friend VectorType operator*(vector_arg v, scalar_arg s)
            {
                return static_foreach<functor_mul>(VectorType(v), s);
            }
            friend VectorType operator*(scalar_arg s, vector_arg v)
            {
                return v * s;
            }
            friend VectorType operator*(vector_arg v1, vector_arg v2)
            {
                return static_foreach<functor_mul>(VectorType(v1), v2);
            }

            friend VectorType operator-(vector_arg v, scalar_arg s)
            {
                return static_foreach<functor_sub>(VectorType(v), s);
            }
            friend VectorType operator-(scalar_arg s, vector_arg v)
            {
                return static_foreach<functor_sub>(VectorType(s), v);
            }
            friend VectorType operator-(vector_arg v1, vector_arg v2)
            {
                return static_foreach<functor_sub>(VectorType(v1), v2);
            }

            friend VectorType operator/(vector_arg v, scalar_arg s)
            {
                return static_foreach<functor_div>(VectorType(v), s);
            }
            friend VectorType operator/(scalar_arg s, vector_arg v)
            {
                return static_foreach<functor_div>(VectorType(s), v);
            }
            inline friend VectorType operator/(vector_arg v1, vector_arg v2)
            {
                return static_foreach<functor_div>(VectorType(v1), v2);
            }
        };
    }
}