#ifndef HEADER_GUARD_SWIZZLE_DETAIL_VECTOR_BINARY_OPERATORS
#define HEADER_GUARD_SWIZZLE_DETAIL_VECTOR_BINARY_OPERATORS

#include <type_traits>
#include "vector_traits.h"

namespace swizzle
{
    namespace detail
    {
        // Remove reference and get the traits
        #define SWIZZLE_DETAIL_TRAITS(T)        vector_traits< typename std::remove_cv< typename std::remove_reference< T >::type >::type >

        // Helper macros
        #define SWIZZLE_DETAIL_SCALAR_PROXY(T)  typename SWIZZLE_DETAIL_TRAITS(T)::fallback_scalar_type
        #define SWIZZLE_DETAIL_RESULT(T)        typename SWIZZLE_DETAIL_TRAITS(T)::result_type
        #define SWIZZLE_DETAIL_VECTOR(T)        typename SWIZZLE_DETAIL_TRAITS(T)::vector_type
        #define SWIZZLE_DETAIL_SCALAR(T)        typename SWIZZLE_DETAIL_TRAITS(T)::scalar_type

        #define SWIZZLE_DETAIL_ARE_SAME(T, U)   are_traits_compatible< typename SWIZZLE_DETAIL_TRAITS(T), typename SWIZZLE_DETAIL_TRAITS(U) >::value

        #define SWIZZLE_DETAIL_RESULT_1(T)       SWIZZLE_DETAIL_RESULT(T)
        #define SWIZZLE_DETAIL_RESULT_2(T, U)    typename std::enable_if< SWIZZLE_DETAIL_ARE_SAME(T, U), SWIZZLE_DETAIL_RESULT(T) >::type
        #define SWIZZLE_DETAIL_RESULT_3(T, U, V) typename std::enable_if< SWIZZLE_DETAIL_ARE_SAME(T, U) && SWIZZLE_DETAIL_ARE_SAME(U, V), SWIZZLE_DETAIL_RESULT(T) >::type

        struct default_vector_unary_operators_tag {};

        template <class Vec1, class Vec2>
        SWIZZLE_DETAIL_RESULT_2(Vec1, Vec2) operator+(Vec1&& a, Vec2&& b)
        {
            return SWIZZLE_DETAIL_VECTOR(Vec1)::add(std::forward<Vec1>(a), std::forward<Vec2>(b));
        }

        template <class Vec1, class Vec2>
        SWIZZLE_DETAIL_RESULT_2(Vec1, Vec2) operator-(Vec1&& a, Vec2&& b)
        {
            return SWIZZLE_DETAIL_VECTOR(Vec1)::sub(std::forward<Vec1>(a), std::forward<Vec2>(b));
        }

        template <class Vec1, class Vec2>
        SWIZZLE_DETAIL_RESULT_2(Vec1, Vec2) operator*(Vec1&& a, Vec2&& b)
        {
            return SWIZZLE_DETAIL_VECTOR(Vec1)::mul(std::forward<Vec1>(a), std::forward<Vec2>(b));
        }

        template <class Vec1, class Vec2>
        SWIZZLE_DETAIL_RESULT_2(Vec1, Vec2) operator/(Vec1&& a, Vec2&& b)
        {
            return SWIZZLE_DETAIL_VECTOR(Vec1)::div(std::forward<Vec1>(a), std::forward<Vec2>(b));
        }
        
        template <class Vec>
        SWIZZLE_DETAIL_RESULT_1(Vec) operator+(Vec&& v, SWIZZLE_DETAIL_SCALAR(Vec) s)
        {
            return SWIZZLE_DETAIL_VECTOR(Vec)::add(std::forward<Vec>(v), s);
        }

        template <class Vec>
        SWIZZLE_DETAIL_RESULT_1(Vec) operator-(Vec&& v, SWIZZLE_DETAIL_SCALAR(Vec) s)
        {
            return SWIZZLE_DETAIL_VECTOR(Vec)::sub(std::forward<Vec>(v), s);
        }

        template <class Vec>
        SWIZZLE_DETAIL_RESULT_1(Vec) operator*(Vec&& v, SWIZZLE_DETAIL_SCALAR(Vec) s)
        {
            return SWIZZLE_DETAIL_VECTOR(Vec)::mul(std::forward<Vec>(v), s);
        }

        template <class Vec>
        SWIZZLE_DETAIL_RESULT_1(Vec) operator/(Vec&& v, SWIZZLE_DETAIL_SCALAR(Vec) s)
        {
            return SWIZZLE_DETAIL_VECTOR(Vec)::div(std::forward<Vec>(v), s);
        }


        template <class Vec>
        SWIZZLE_DETAIL_RESULT_1(Vec) operator+(SWIZZLE_DETAIL_SCALAR(Vec) s, Vec&& v)
        {
            return SWIZZLE_DETAIL_VECTOR(Vec)::add(std::forward<Vec>(v), s);
        }

        template <class Vec>
        SWIZZLE_DETAIL_RESULT_1(Vec) operator-(SWIZZLE_DETAIL_SCALAR(Vec) s, Vec&& v)
        {
            return SWIZZLE_DETAIL_VECTOR(Vec)::sub(SWIZZLE_DETAIL_VECTOR(Vec)(s), std::forward<Vec>(v));
        }

        template <class Vec>
        SWIZZLE_DETAIL_RESULT_1(Vec) operator*(SWIZZLE_DETAIL_SCALAR(Vec) s, Vec&& v)
        {
            return SWIZZLE_DETAIL_VECTOR(Vec)::mul(std::forward<Vec>(v), s);
        }

        template <class Vec>
        SWIZZLE_DETAIL_RESULT_1(Vec) operator/(SWIZZLE_DETAIL_SCALAR(Vec) s, Vec&& v)
        {
            return SWIZZLE_DETAIL_VECTOR(Vec)::div(SWIZZLE_DETAIL_VECTOR(Vec)(s), std::forward<Vec>(v));
        }
    }
}

#endif  HEADER_GUARD_SWIZZLE_DETAIL_VECTOR_BINARY_OPERATORS
