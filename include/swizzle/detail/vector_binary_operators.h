#ifndef HEADER_GUARD_SWIZZLE_DETAIL_VECTOR_BINARY_OPERATORS
#define HEADER_GUARD_SWIZZLE_DETAIL_VECTOR_BINARY_OPERATORS

#include <type_traits>
#include "vector_traits.h"

namespace swizzle
{
    namespace detail
    {
        #define SWIZZLE_DETAIL_TRAITS(T) vector_traits<typename std::remove_reference<T>::type>

        #define SWIZZLE_DETAIL_VECTOR(T) typename SWIZZLE_DETAIL_TRAITS(T)::vector_type
        #define SWIZZLE_DETAIL_SCALAR(T) typename SWIZZLE_DETAIL_TRAITS(T)::scalar_type
        #define SWIZZLE_DETAIL_NUM(T) SWIZZLE_DETAIL_TRAITS(T)::num_of_components
        #define SWIZZLE_DETAIL_SCALAR_PROXY(T) typename SWIZZLE_DETAIL_TRAITS(T)::vector_type::fallback_scalar_type

        #define SWIZZLE_DETAIL_ENABLE_IF_SAME2(T, U, Out) typename std::enable_if< \
            std::is_same<SWIZZLE_DETAIL_VECTOR(T), SWIZZLE_DETAIL_VECTOR(U)>::value, Out>::type
        #define SWIZZLE_DETAIL_ENABLE_IF_SAME(T, U) SWIZZLE_DETAIL_ENABLE_IF_SAME2(T, U, SWIZZLE_DETAIL_VECTOR(T))

        namespace
        {
            //! A forwarder for add operators - arguments are passed, copied or casted to, depending on context.
            template <class T>
            T add_forwarder(T a, const T& b)
            {
                return a += b;
            }

            //! A forwarder for sub operators - arguments are passed, copied or casted to, depending on context.
            template <class T>
            T sub_forwarder(T a, const T& b)
            {
                return a -= b;
            }

            //! A forwarder for mul operators - arguments are passed, copied or casted to, depending on context.
            template <class T>
            T mul_forwarder(T a, const T& b)
            {
                return a *= b;
            }

            //! A forwarder for div operators - arguments are passed, copied or casted to, depending on context.
            template <class T>
            T div_forwarder(T a, const T& b)
            {
                return a /= b;
            }
        }

        struct default_vector_unary_operators_tag {};

        template <class TVector1, class TVector2>
        SWIZZLE_DETAIL_ENABLE_IF_SAME(TVector1, TVector2) operator+(const TVector1& a, const TVector2& b)
        {
            return add_forwarder<SWIZZLE_DETAIL_VECTOR(TVector1)>(a, b);
        }

        template <class TVector1, class TVector2>
        typename std::enable_if< 
            !std::is_same<SWIZZLE_DETAIL_VECTOR(TVector1), SWIZZLE_DETAIL_VECTOR(TVector2)>::value &&  
            SWIZZLE_DETAIL_TRAITS(TVector1)::num_of_components == SWIZZLE_DETAIL_TRAITS(TVector2)::num_of_components,
            SWIZZLE_DETAIL_VECTOR(TVector1)>::type
            operator+(const TVector1& a, const TVector2& b)
        {
            return add_forwarder<SWIZZLE_DETAIL_VECTOR(TVector1)>(a, static_cast<SWIZZLE_DETAIL_VECTOR(TVector1)>(b));
        }

        template <class TVector1, class TVector2>
        SWIZZLE_DETAIL_ENABLE_IF_SAME(TVector1, TVector2) operator-(const TVector1& a, const TVector2& b)
        {
            return sub_forwarder<SWIZZLE_DETAIL_VECTOR(TVector1)>(a, b);
        }

        template <class TVector1, class TVector2>
        SWIZZLE_DETAIL_ENABLE_IF_SAME(TVector1, TVector2) operator*(const TVector1& a, const TVector2& b)
        {
            return mul_forwarder<SWIZZLE_DETAIL_VECTOR(TVector1)>(a, b);
        }

        template <class TVector1, class TVector2>
        SWIZZLE_DETAIL_ENABLE_IF_SAME(TVector1, TVector2) operator/(const TVector1& a, const TVector2& b)
        {
            return div_forwarder<SWIZZLE_DETAIL_VECTOR(TVector1)>(a, b);
        }
        
        template <class TVector>
        SWIZZLE_DETAIL_VECTOR(TVector) operator+(const TVector& v, const SWIZZLE_DETAIL_SCALAR_PROXY(TVector)& s)
        {
            return add_forwarder<SWIZZLE_DETAIL_VECTOR(TVector)>(v, SWIZZLE_DETAIL_VECTOR(TVector)::from_scalar(s));
        }

        template <class TVector>
        SWIZZLE_DETAIL_VECTOR(TVector) operator-(const TVector& v, const SWIZZLE_DETAIL_SCALAR_PROXY(TVector)& s)
        {
            return sub_forwarder<SWIZZLE_DETAIL_VECTOR(TVector)>(v, SWIZZLE_DETAIL_VECTOR(TVector)::from_scalar(s));
        }

        template <class TVector>
        SWIZZLE_DETAIL_VECTOR(TVector) operator*(const TVector& v, const SWIZZLE_DETAIL_SCALAR_PROXY(TVector)& s)
        {
            return mul_forwarder<SWIZZLE_DETAIL_VECTOR(TVector)>(v, SWIZZLE_DETAIL_VECTOR(TVector)::from_scalar(s));
        }

        template <class TVector>
        SWIZZLE_DETAIL_VECTOR(TVector) operator/(const TVector& v, const SWIZZLE_DETAIL_SCALAR_PROXY(TVector)& s)
        {
            return div_forwarder<SWIZZLE_DETAIL_VECTOR(TVector)>(v, SWIZZLE_DETAIL_VECTOR(TVector)::from_scalar(s));
        }


        template <class TVector>
        SWIZZLE_DETAIL_VECTOR(TVector) operator+(const SWIZZLE_DETAIL_SCALAR_PROXY(TVector)& s, const TVector& v)
        {
            return add_forwarder<SWIZZLE_DETAIL_VECTOR(TVector)>(SWIZZLE_DETAIL_VECTOR(TVector)::from_scalar(s), v);
        }

        template <class TVector>
        SWIZZLE_DETAIL_VECTOR(TVector) operator-(const SWIZZLE_DETAIL_SCALAR_PROXY(TVector)& s, const TVector& v)
        {
            return sub_forwarder<SWIZZLE_DETAIL_VECTOR(TVector)>(SWIZZLE_DETAIL_VECTOR(TVector)::from_scalar(s), v);
        }

        template <class TVector>
        SWIZZLE_DETAIL_VECTOR(TVector) operator*(const SWIZZLE_DETAIL_SCALAR_PROXY(TVector)& s, const TVector& v)
        {
            return mul_forwarder<SWIZZLE_DETAIL_VECTOR(TVector)>(SWIZZLE_DETAIL_VECTOR(TVector)::from_scalar(s), v);
        }

        template <class TVector>
        SWIZZLE_DETAIL_VECTOR(TVector) operator/(const SWIZZLE_DETAIL_SCALAR_PROXY(TVector)& s, const TVector& v)
        {
            return div_forwarder<SWIZZLE_DETAIL_VECTOR(TVector)>(SWIZZLE_DETAIL_VECTOR(TVector)::from_scalar(s), v);
        }
    }
}

#endif  HEADER_GUARD_SWIZZLE_DETAIL_VECTOR_BINARY_OPERATORS
