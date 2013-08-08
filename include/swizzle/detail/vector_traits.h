#ifndef HEADER_GUARD_SWIZZLE_DETAIL_VECTOR_TRAITS
#define HEADER_GUARD_SWIZZLE_DETAIL_VECTOR_TRAITS

#include <type_traits>
#include "writable_wrapper.h"

namespace swizzle
{
    namespace detail
    {
        template <class T>
        struct remove_reference_cv : std::remove_cv< typename std::remove_reference<T>::type > {};

        struct nothing {};

        template <class T>
        struct get_vector_type_impl
        {};

        template <class TBase>
        struct get_vector_type_impl< writable_wrapper<TBase> >
            : get_vector_type_impl< TBase >
        {};


        template <class T, class U = void, class V = void>
        struct get_vector_type;

        template <class T>
        struct get_vector_type<T, void, void>
        {
            typedef typename swizzle::detail::get_vector_type_impl< typename remove_reference_cv<T>::type >::type type;
        };

        template <class T, class U>
        struct get_vector_type<T, U, void>
        {
            typedef typename get_vector_type<T>::type type_1;
            typedef typename get_vector_type<U>::type type_2;

            typedef typename type_1::scalar_type scalar_type_1;
            typedef typename type_1::scalar_type scalar_type_2;
            typedef typename std::common_type<scalar_type_1, scalar_type_2>::type common_scalar_type;

            static const bool are_sizes_equal = type_1::num_of_components == type_2::num_of_components;
            static const bool is_first_bigger = type_1::num_of_components > type_2::num_of_components;

            // this may be confusing at first, but principle is simple
            // sizes match -> type that scalars get promoted to gets chosen
            // sizes don't match -> type that has bigger number of components wins
            typedef typename std::conditional<
                // this may be confusing at first, but principle is simple; first vector type gets chosen if and only if either it has bigger number
                // of components or if number is same, but scalar types' common type is equal to the first one
                (are_sizes_equal && std::is_same<scalar_type_1, common_scalar_type>::value) || is_first_bigger,
                type_1,
                type_2
            >::type type;
        };

        template <class T, class U, class V>
        struct get_vector_type
        {
            typedef typename get_vector_type< typename get_vector_type<T, U>::type, V >::type type;
        };

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


    }
}

#endif // HEADER_GUARD_SWIZZLE_DETAIL_VECTOR_TRAITS
