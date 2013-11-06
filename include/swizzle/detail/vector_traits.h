// CxxSwizzle
// Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <type_traits>
#include <swizzle/detail/utils.h>

namespace swizzle
{
    namespace detail
    {
        //! Type to specialise; it should define a nested type 'type' being a vector
        //! Non-specialised version does not define it, failing any function relying on it.
        template <class T>
        struct get_vector_type_impl
        {};

        //! Used for graceful SFINAE - becomes true_type if get_vector_type_impl defines nested type.
        template <class T>
        struct has_vector_type_impl : std::integral_constant<bool, has_type< get_vector_type_impl< typename remove_reference_cv<T>::type > >::value >
        {};



        //! A generic common vector type implementation. Defines a type if:
        //! 1) scalars have a common type and it must be either one of them
        //! 2) Sizes are equal
        //! This type is specialised to handle cases of 1-sized vectors.
        template <class VectorType1, class ScalarType1, size_t Size1, class VectorType2, class ScalarType2, size_t Size2>
        struct common_vector_type_generic_fallback
        {
        private:
            typedef typename std::common_type<ScalarType1, ScalarType2>::type common_scalar_type;
            static_assert( std::is_same<common_scalar_type, ScalarType1>::value || std::is_same<common_scalar_type, ScalarType2>::value, 
                "Common type must be same as at least one of scalar types");
            static_assert( Size1 == Size2 || Size1 == 1 || Size2 == 1,
                "Either both vectors must have same size or either one of them has to have a size equal to 1 (auto promotion to the bigger vector)");

        public:
            typedef typename std::conditional< Size1 == Size2,
                typename std::conditional< std::is_same<common_scalar_type, ScalarType1>::value,
                    VectorType1,
                    VectorType2
                >::type,
                typename std::conditional< Size2 == 1,
                    VectorType1,
                    VectorType2
                >::type
            >::type type;
        };

        //! Defines a common type for two vectors. By default falls back to common_vector_type_generic_fallback,
        //! but can be easily specialised to handle vectors differently.
        template <class VectorType1, class VectorType2>
        struct common_vector_type : common_vector_type_generic_fallback<
            VectorType1, typename VectorType1::scalar_type, VectorType1::num_of_components, 
            VectorType2, typename VectorType2::scalar_type, VectorType2::num_of_components>
        {};



        //! Defines common vector type for given combination of input types; should be done using variadic templates,
        //! but MSVC does not support them. Scalars are treated as one-component vector.
        //! Anyway, this type is specialised later on.
        template <class T, class U = void, class V = void, class = std::true_type >
        struct get_vector_type
        {};

        //! A specialisation for one type; redirects to get_vector_type_impl<T>
        template <class T>
        struct get_vector_type<T, void, void, std::integral_constant<bool, has_vector_type_impl<T>::value> > 
            : get_vector_type_impl< typename remove_reference_cv<T>::type >
        {};

        //! A specialisation for two types; redirectrs to common_vector_type
        template <class T, class U>
        struct get_vector_type<T, U, void, std::integral_constant<bool, has_vector_type_impl<T>::value && has_vector_type_impl<U>::value> > 
            : common_vector_type< typename get_vector_type<T>::type, typename get_vector_type<U>::type >
        {};

        //! Uses two-types specialization to get the result
        template <class T, class U, class V>
        struct get_vector_type<T, U, V, std::integral_constant<bool, has_vector_type_impl<T>::value && has_vector_type_impl<U>::value && has_vector_type_impl<V>::value> > 
            : get_vector_type< typename get_vector_type<T, U>::type, V >
        {};



        //! A shortcut for getting the number of vector's components.
        template <class T, class = std::true_type >
        struct get_vector_size
        {};

        //! A shortcut for getting the number of vector's components.
        template <>
        struct get_vector_size< nothing, std::true_type > : std::integral_constant<size_t, 0>
        {};

        template <class T>
        struct get_vector_size<T, std::integral_constant<bool, is_greater<get_vector_type<T>::type::num_of_components, 0>::value> >
            :  std::integral_constant< size_t, get_vector_type<T>::type::num_of_components >
        {};

        // damn you MSVC and your lack of variadic templates....

        template <class T1, class T2 = nothing, class T3 = nothing, class T4 = nothing, 
                  class T5 = nothing, class T6 = nothing, class T7 = nothing, class T8 = nothing, 
                  class T9 = nothing, class T10 = nothing, class T11 = nothing, class T12 = nothing, 
                  class T13 = nothing, class T14 = nothing, class T15 = nothing, class T16 = nothing,
                  class = std::true_type>
        struct get_total_size
        {};

        template <class T1, class T2, class T3, class T4, 
                  class T5, class T6, class T7, class T8, 
                  class T9, class T10, class T11, class T12, 
                  class T13, class T14, class T15, class T16>
        struct get_total_size<
            T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16,
            std::integral_constant<bool, 0 <
                get_vector_size<T1>::value + 
                get_vector_size<T2>::value + 
                get_vector_size<T3>::value + 
                get_vector_size<T4>::value + 
                get_vector_size<T5>::value + 
                get_vector_size<T6>::value + 
                get_vector_size<T7>::value + 
                get_vector_size<T8>::value + 
                get_vector_size<T9>::value + 
                get_vector_size<T10>::value + 
                get_vector_size<T11>::value + 
                get_vector_size<T12>::value + 
                get_vector_size<T13>::value + 
                get_vector_size<T14>::value + 
                get_vector_size<T15>::value + 
                get_vector_size<T16>::value>
            > :
            std::integral_constant<size_t, 
                get_vector_size<T1>::value + 
                get_vector_size<T2>::value + 
                get_vector_size<T3>::value + 
                get_vector_size<T4>::value + 
                get_vector_size<T5>::value + 
                get_vector_size<T6>::value + 
                get_vector_size<T7>::value + 
                get_vector_size<T8>::value + 
                get_vector_size<T9>::value + 
                get_vector_size<T10>::value + 
                get_vector_size<T11>::value + 
                get_vector_size<T12>::value + 
                get_vector_size<T13>::value + 
                get_vector_size<T14>::value + 
                get_vector_size<T15>::value + 
                get_vector_size<T16>::value>
        {};
    }
}