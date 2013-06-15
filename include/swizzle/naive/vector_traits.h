#ifndef HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_TRAITS
#define HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_TRAITS

#include <type_traits>
#include "../detail/writable_wrapper.h"
#include "../detail/vector_traits.h"

namespace swizzle
{
    namespace naive
    {
        template <class TScalar, size_t Num>
        class vector_adapter;

        template <class TVector, class TScalar, size_t NumOfComponents, size_t x, size_t y, size_t z, size_t w>
        struct vector_proxy;
    }

    namespace detail
    {
        struct not_available
        {
        private:
            not_available();
        };

        struct not_available2
        {
        private:
            not_available2();
        };

        template <class T, bool isArithmetic>
        struct vector_traits_fallback
        {

        };

        template <class T>
        struct vector_traits_fallback<T, true>
        {
            static const size_t num_of_components = 1;

            typedef naive::vector_adapter< T, 1 > vector_type;
            typedef T scalar_type;
            typedef T fallback_scalar_type;
            typedef T result_type;
        };

        template <class T>
        struct vector_traits : vector_traits_fallback<T, std::is_arithmetic<T>::value>
        {
        };

        template <class TScalar, size_t Num>
        struct vector_traits< naive::vector_adapter<TScalar, Num> >
        {
            static const size_t num_of_components = Num;
            typedef naive::vector_adapter<TScalar, Num> vector_type;
            typedef TScalar scalar_type;
            typedef typename vector_type::fallback_scalar_type fallback_scalar_type;
            typedef vector_type result_type;
        };

        template <class TVector, class TScalar, size_t NumOfComponents, size_t x, size_t y, size_t z, size_t w>
        struct vector_traits< naive::vector_proxy<TVector, TScalar, NumOfComponents, x, y, z, w> >
        {
            static const size_t num_of_components = TVector::num_of_components;
            typedef TVector vector_type;
            typedef TScalar scalar_type;
            typedef typename TVector::fallback_scalar_type fallback_scalar_type;
            typedef vector_type result_type;
        };

        template <class TBase>
        struct vector_traits< detail::writable_wrapper<TBase> >
        {
            static const size_t num_of_components = TBase::num_of_components;
            typedef typename TBase::vector_type vector_type;
            typedef typename TBase::scalar_type scalar_type;
            typedef typename vector_type::fallback_scalar_type fallback_scalar_type;
            typedef typename vector_type::result_type result_type;
        };
    }
}


#endif  HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_TRAITS
