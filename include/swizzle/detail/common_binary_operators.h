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
        template <typename VectorType, typename ScalarType, typename VectorArgType = const VectorType&, typename ScalarArgType = const ScalarType&>
        struct common_binary_operators
        {
            typedef VectorArgType vector_arg;
            typedef ScalarArgType scalar_arg;

            friend VectorType operator+(vector_arg v, scalar_arg s)
            {
                VectorType result(v);
                return static_foreach<functor_add>(result, s);
            }
            friend VectorType operator+(scalar_arg s, vector_arg v)
            {
                return v + s;
            }
            friend VectorType operator+(vector_arg v1, vector_arg v2)
            {
                VectorType result(v1);
                return static_foreach<functor_add>(result, v2);
            }

            friend VectorType operator*(vector_arg v, scalar_arg s)
            {
                VectorType result(v);
                return static_foreach<functor_mul>(result, s);
            }
            friend VectorType operator*(scalar_arg s, vector_arg v)
            {
                return v * s;
            }
            friend VectorType operator*(vector_arg v1, vector_arg v2)
            {
                VectorType result(v1);
                return static_foreach<functor_mul>(result, v2);
            }

            friend VectorType operator-(vector_arg v, scalar_arg s)
            {
                VectorType result(v);
                return static_foreach<functor_sub>(result, s);
            }
            friend VectorType operator-(scalar_arg s, vector_arg v)
            {
                VectorType result(s);
                return static_foreach<functor_sub>(result, v);
            }
            friend VectorType operator-(vector_arg v1, vector_arg v2)
            {
                VectorType result(v1);
                return static_foreach<functor_sub>(result, v2);
            }

            friend VectorType operator/(vector_arg v, scalar_arg s)
            {
                VectorType result(v);
                return static_foreach<functor_div>(result, s);
            }
            friend VectorType operator/(scalar_arg s, vector_arg v)
            {
                VectorType result(s);
                return static_foreach<functor_div>(result, v);
            }
            inline friend VectorType operator/(vector_arg v1, vector_arg v2)
            {
                VectorType result(v1);
                return static_foreach<functor_div>(result, v2);
            }
        };
    }
}