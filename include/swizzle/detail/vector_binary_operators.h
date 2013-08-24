#pragma once

#include "vector_traits.h"

namespace swizzle
{
    namespace detail
    {
        namespace binary_operators
        {
            struct tag {};

            template <class T>
            typename get_vector_type_no_scalars_strong<T>::type operator+(T&& t, typename get_vector_type<T>::type::scalar_type a)
            {
                return typename get_vector_type<T>::type( std::forward<T>(t) ) += a;
            }

            template <class T>
            typename get_vector_type_no_scalars_strong<T>::type operator-(T&& t, typename get_vector_type<T>::type::scalar_type a)
            {
                return typename get_vector_type<T>::type( std::forward<T>(t) ) -= a;
            }

            template <class T>
            typename get_vector_type_no_scalars_strong<T>::type operator*(T&& t, typename get_vector_type<T>::type::scalar_type a)
            {
                return typename get_vector_type<T>::type( std::forward<T>(t) ) *= a;
            }

            template <class T>
            typename get_vector_type_no_scalars_strong<T>::type operator/(T&& t, typename get_vector_type<T>::type::scalar_type a)
            {
                return typename get_vector_type<T>::type( std::forward<T>(t) ) /= a;
            }


            template <class T>
            typename get_vector_type_no_scalars_strong<T>::type operator+(typename get_vector_type<T>::type::scalar_type a, T&& t)
            {
                return std::forward<T>(t) + a;
            }

            template <class T>
            typename get_vector_type_no_scalars_strong<T>::type operator-(typename get_vector_type<T>::type::scalar_type a, T&& t)
            {
                return typename get_vector_type<T>::type( a ) -= std::forward<T>(t);
            }

            template <class T>
            typename get_vector_type_no_scalars_strong<T>::type operator*(typename get_vector_type<T>::type::scalar_type a, T&& t)
            {
                return std::forward<T>(t) * a;
            }

            template <class T>
            typename get_vector_type_no_scalars_strong<T>::type operator/(typename get_vector_type<T>::type::scalar_type a, T&& t)
            {
                return typename get_vector_type<T>::type( a ) /= std::forward<T>(t);
            }


            template <class T, class U>
            typename get_vector_type_no_scalars<T, U>::type operator+(T&& t, U&& u)
            {
                return typename get_vector_type<T, U>::type( std::forward<T>(t) ) += std::forward<U>(u);
            }

            template <class T, class U>
            typename get_vector_type_no_scalars<T, U>::type operator-(T&& t, U&& u)
            {
                return typename get_vector_type<T, U>::type( std::forward<T>(t) ) -= std::forward<U>(u);
            }

            template <class T, class U>
            typename get_vector_type_no_scalars<T, U>::type operator*(T&& t, U&& u)
            {
                return typename get_vector_type<T, U>::type( std::forward<T>(t) ) *= std::forward<U>(u);
            }

            template <class T, class U>
            typename get_vector_type_no_scalars<T, U>::type operator/(T&& t, U&& u)
            {
                return typename get_vector_type<T, U>::type( std::forward<T>(t) ) /= std::forward<U>(u);
            }
        }
    }
}