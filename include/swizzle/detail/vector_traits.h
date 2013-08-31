#pragma once

#include <type_traits>
#include "utils.h"

namespace swizzle
{
    namespace detail
    {
        //! Type to specialise; it should define a nested type 'type' being a vector
        //! Non-specialised version does not define it, failing any function relying on it
        template <class T>
        struct get_vector_type_impl;

        //! Type needs specializing for custom vectors
        template <class T, class U>
        struct common_vector_type
        {};


        //! Defines common vector for given combination of input types; should be done using variadic templates,
        //! but MSVC does not support them
        template <class T, class U = void, class V = void>
        struct get_vector_type;

        //! A specialisation for one type; redirects to get_vector_type_impl<T>
        template <class T>
        struct get_vector_type<T, void, void> : get_vector_type_impl< typename remove_reference_cv<T>::type >
        {};

        //! A specialisation for two types; redirectrs to common_vector_type
        template <class T, class U>
        struct get_vector_type<T, U, void> : common_vector_type< typename get_vector_type<T>::type, typename get_vector_type<U>::type >
        {};

        //! Uses two-types specialization to get the result
        template <class T, class U, class V>
        struct get_vector_type : get_vector_type< typename get_vector_type<T, U>::type, V >
        {};

        //! Defines a common vector type ONLY if none of type arguments is a scalar, i.e. no implic conversion scalar->vector takes place;
        //! Useful when defining operators
        template <class T, class U = void, class V = void>
        struct get_vector_type_no_scalars_strong :
            std::conditional<
                !std::is_arithmetic<typename remove_reference_cv<T>::type>::value &&
                !std::is_arithmetic<typename remove_reference_cv<U>::type>::value &&
                !std::is_arithmetic<typename remove_reference_cv<V>::type>::value &&
                get_vector_type<T, U, V>::type::num_of_components != 1,
                get_vector_type<T, U, V>,
                nothing
            >::type
        {};

        template <class T, class U = void, class V = void>
        struct get_vector_type_no_scalars :
            std::conditional<
                !std::is_arithmetic<typename remove_reference_cv<T>::type>::value &&
                !std::is_arithmetic<typename remove_reference_cv<U>::type>::value &&
                !std::is_arithmetic<typename remove_reference_cv<V>::type>::value,
                get_vector_type<T, U, V>,
                nothing
            >::type
        {};


        //! A shortcut for getting the number of vector's components.
        template <class T >
        struct get_vector_size : std::integral_constant< size_t, get_vector_type<T>::type::num_of_components >
        {};
    }
}