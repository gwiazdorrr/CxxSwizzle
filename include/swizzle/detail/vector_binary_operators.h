#ifndef HEADER_GUARD_SWIZZLE_DETAIL_VECTOR_BINARY_OPERATORS
#define HEADER_GUARD_SWIZZLE_DETAIL_VECTOR_BINARY_OPERATORS

#include <type_traits>
#include "vector_traits.h"

namespace swizzle
{
    namespace detail
    {
        #define SWIZZLE_DETAIL_TRAITS(T) vector_traits<typename std::remove_reference<T>::type>

        #define SWIZZLE_DETAIL_SCALAR_PROXY(T) typename SWIZZLE_DETAIL_TRAITS(T)::fallback_scalar_type
        #define SWIZZLE_DETAIL_RESULT(T) typename SWIZZLE_DETAIL_TRAITS(T)::result_type
        #define SWIZZLE_DETAIL_VECTOR(T) typename SWIZZLE_DETAIL_TRAITS(T)::vector_type
        #define SWIZZLE_DETAIL_SCALAR(T) typename SWIZZLE_DETAIL_TRAITS(T)::scalar_type

        #define SWIZZLE_DETAIL_NUM(T) SWIZZLE_DETAIL_TRAITS(T)::num_of_components
        

        #define SWIZZLE_DETAIL_ENABLE_IF_SAME2(T, U, Out) typename std::enable_if< \
            std::is_same<SWIZZLE_DETAIL_VECTOR(T), SWIZZLE_DETAIL_VECTOR(U)>::value, Out>::type

        #define SWIZZLE_DETAIL_ENABLE_IF_SAME(T, U) SWIZZLE_DETAIL_ENABLE_IF_SAME2(T, U, SWIZZLE_DETAIL_RESULT(T))


        struct default_vector_unary_operators_tag {};

        template <class TVector1, class TVector2>
        SWIZZLE_DETAIL_ENABLE_IF_SAME(TVector1, TVector2) operator+(const TVector1& a, TVector2&& b)
        {
            return SWIZZLE_DETAIL_VECTOR(TVector1)::add(a, b);
        }

        template <class TVector1, class TVector2>
        SWIZZLE_DETAIL_ENABLE_IF_SAME(TVector1, TVector2) operator-(const TVector1& a, TVector2&& b)
        {
            return SWIZZLE_DETAIL_VECTOR(TVector1)::sub(a, b);
        }

        template <class TVector1, class TVector2>
        SWIZZLE_DETAIL_ENABLE_IF_SAME(TVector1, TVector2) operator*(const TVector1& a, TVector2&& b)
        {
            return SWIZZLE_DETAIL_VECTOR(TVector1)::mul(a, b);
        }

        template <class TVector1, class TVector2>
        SWIZZLE_DETAIL_ENABLE_IF_SAME(TVector1, TVector2) operator/(const TVector1& a, TVector2&& b)
        {
            return SWIZZLE_DETAIL_VECTOR(TVector1)::div(a, b);
        }
        
        template <class TVector>
        SWIZZLE_DETAIL_RESULT(TVector) operator+(const TVector& v, const SWIZZLE_DETAIL_SCALAR_PROXY(TVector)& s)
        {
            return SWIZZLE_DETAIL_VECTOR(TVector)::add(v, SWIZZLE_DETAIL_VECTOR(TVector)(s));
        }

        template <class TVector>
        SWIZZLE_DETAIL_RESULT(TVector) operator-(const TVector& v, const SWIZZLE_DETAIL_SCALAR_PROXY(TVector)& s)
        {
            return SWIZZLE_DETAIL_VECTOR(TVector)::sub(v, SWIZZLE_DETAIL_VECTOR(TVector)(s));
        }

        template <class TVector>
        SWIZZLE_DETAIL_RESULT(TVector) operator*(const TVector& v, const SWIZZLE_DETAIL_SCALAR_PROXY(TVector)& s)
        {
            return SWIZZLE_DETAIL_VECTOR(TVector)::mul(v, SWIZZLE_DETAIL_VECTOR(TVector)(s));
        }

        template <class TVector>
        SWIZZLE_DETAIL_RESULT(TVector) operator/(const TVector& v, const SWIZZLE_DETAIL_SCALAR_PROXY(TVector)& s)
        {
            return SWIZZLE_DETAIL_VECTOR(TVector)::div(v, SWIZZLE_DETAIL_VECTOR(TVector)(s));
        }


        template <class TVector>
        SWIZZLE_DETAIL_RESULT(TVector) operator+(const SWIZZLE_DETAIL_SCALAR_PROXY(TVector)& s, const TVector& v)
        {
            return SWIZZLE_DETAIL_VECTOR(TVector)::add(SWIZZLE_DETAIL_VECTOR(TVector)(s), v);
        }

        template <class TVector>
        SWIZZLE_DETAIL_RESULT(TVector) operator-(const SWIZZLE_DETAIL_SCALAR_PROXY(TVector)& s, const TVector& v)
        {
            return SWIZZLE_DETAIL_VECTOR(TVector)::sub(SWIZZLE_DETAIL_VECTOR(TVector)(s), v);
        }

        template <class TVector>
        SWIZZLE_DETAIL_RESULT(TVector) operator*(const SWIZZLE_DETAIL_SCALAR_PROXY(TVector)& s, const TVector& v)
        {
            return SWIZZLE_DETAIL_VECTOR(TVector)::mul(SWIZZLE_DETAIL_VECTOR(TVector)(s), v);
        }

        template <class TVector>
        SWIZZLE_DETAIL_RESULT(TVector) operator/(const SWIZZLE_DETAIL_SCALAR_PROXY(TVector)& s, const TVector& v)
        {
            return SWIZZLE_DETAIL_VECTOR(TVector)::div(SWIZZLE_DETAIL_VECTOR(TVector)(s), v);
        }
    }
}

#endif  HEADER_GUARD_SWIZZLE_DETAIL_VECTOR_BINARY_OPERATORS
