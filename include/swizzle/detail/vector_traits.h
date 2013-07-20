#ifndef HEADER_GUARD_SWIZZLE_DETAIL_VECTOR_TRAITS
#define HEADER_GUARD_SWIZZLE_DETAIL_VECTOR_TRAITS

#include <type_traits>

namespace swizzle
{
    namespace detail
    {
        //! A base for vector traits. All valid specialisations of vector_traits should derive from it.
        template <class TVector>
        struct vector_traits_base
        {
            typedef TVector vector_type;
            typedef typename TVector::scalar_type scalar_type;
            typedef typename TVector::result_type result_type;
            typedef typename TVector::fallback_scalar_type fallback_scalar_type;
            static const size_t num_of_components = TVector::num_of_components;
        };

        //! An undefined class, specialisations must derive from vector_traits_base.
        template <class T>
        struct vector_traits;

        //! Helper type, checks if two traits are compatible. Default version is undefined.
        template <class TTraits1, class TTraits2>
        struct are_traits_compatible;

        //! A specialisation for vector_traits. If scalar types and number of components are equal then
        //! two traits are compatible.
        template <class T, class U>
        struct are_traits_compatible< vector_traits<T>, vector_traits<U> >
        {
            typedef vector_traits<T> traits_1 ;
            typedef vector_traits<U> traits_2 ;

            static const bool value = 
                std::is_same< typename traits_1::scalar_type, typename traits_2::scalar_type >::value &&
                traits_1::num_of_components == traits_2::num_of_components;

        };
    }
}

#endif  HEADER_GUARD_SWIZZLE_DETAIL_VECTOR_TRAITS
