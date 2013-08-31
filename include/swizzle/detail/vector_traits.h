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
        struct get_vector_type_impl
        {};

        
        template <class T>
        struct is_vector : std::integral_constant<bool, has_type< get_vector_type_impl< typename remove_reference_cv<T>::type > >::value >
        {};

        //! Defines common vector for given combination of input types; should be done using variadic templates,
        //! but MSVC does not support them
        template <class T, class U = void, class V = void, class = std::true_type >
        struct get_vector_type
        {};

 
        //! Type needs specializing for custom vectors
        template <class T, class U>
        struct common_vector_type //{};

        {
            typedef typename get_vector_type<T>::type type_1;
            typedef typename get_vector_type<U>::type type_2;

            typedef typename type_1::scalar_type scalar_type_1;
            typedef typename type_1::scalar_type scalar_type_2;
            typedef typename std::common_type<scalar_type_1, scalar_type_2>::type common_scalar_type;

            //! had to do comparisons outside std::conditional as using less than/greater than in templates confuses compiler
            static const bool are_sizes_equal = type_1::num_of_components == type_2::num_of_components;
            static const bool is_first_bigger = type_1::num_of_components > type_2::num_of_components;

            // this may be confusing at first, but principle is simple
            // sizes match -> type that scalars get promoted to gets chosen
            // sizes don't match -> type that has bigger number of components wins
            typedef typename std::conditional<
                are_sizes_equal && std::is_same<scalar_type_1, common_scalar_type>::value || is_first_bigger,
                type_1,
                type_2
            >::type type;
        };



        //! A specialisation for one type; redirects to get_vector_type_impl<T>
        template <class T>
        struct get_vector_type<T, void, void, std::integral_constant<bool, is_vector<T>::value> > 
            : get_vector_type_impl< typename remove_reference_cv<T>::type >
        {};

        //! A specialisation for two types; redirectrs to common_vector_type
        template <class T, class U>
        struct get_vector_type<T, U, void, std::integral_constant<bool, is_vector<T>::value && is_vector<U>::value> > 
            : common_vector_type< typename get_vector_type<T>::type, typename get_vector_type<U>::type >
        {};

        //! Uses two-types specialization to get the result
        template <class T, class U, class V>
        struct get_vector_type<T, U, V, std::integral_constant<bool, is_vector<T>::value && is_vector<U>::value && is_vector<V>::value> > 
            : get_vector_type< typename get_vector_type<T, U>::type, V >
        {};



        template <class T, class U = void, class = std::true_type>
        struct get_vector_type_no_scalars
        {};

        template <class T, class U>
        struct get_vector_type_no_scalars<
            T, U,
            std::integral_constant<
                bool,
                !std::is_arithmetic<typename remove_reference_cv<T>::type>::value &&
                !std::is_arithmetic<typename remove_reference_cv<U>::type>::value
            >
        > : get_vector_type<T, U>
        {};

        //! Defines a common vector type ONLY if none of type arguments is a scalar, i.e. no implic conversion scalar->vector takes place;
        //! Useful when defining operators
        template <class T, class = std::true_type>
        struct get_vector_type_no_scalars_strong
        {};


        template <class T>
        struct get_vector_type_no_scalars_strong<
            T,
            std::integral_constant< 
                bool,
                get_vector_type<T>::type::num_of_components != 1
            > 
        > : get_vector_type<T>
        {};


        //! A shortcut for getting the number of vector's components.
        template <class T, class = std::true_type >
        struct get_vector_size
        {};

        template <class T>
        struct get_vector_size<T, std::integral_constant<bool, is_greater<get_vector_type<T>::type::num_of_components, 0>::value> >
            :  std::integral_constant< size_t, get_vector_type<T>::type::num_of_components >
        {
        };
    }
}