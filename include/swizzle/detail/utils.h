// CxxSwizzle
// Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <type_traits>
#include <cstddef>

namespace swizzle
{
    namespace detail
    {
        //! Removes reference and const/volatile
        template <class T>
        struct remove_reference_cv : std::remove_cv< typename std::remove_reference<T>::type > {};

        //! MSVC does not come with this in the STL.
        template< class T >
        typename std::add_rvalue_reference<T>::type declval();

        //! Detects presence of nested "typedef <something> type"
        template <typename T>
        struct has_type_helper {
        private:
            template <typename T1> static typename T1::type test(int);
            template <typename> static void test(...);
        public:
            static const bool value = !std::is_void<decltype(test<T>(0))>::value;
        };

        template <typename T>
        struct has_type : std::integral_constant< bool, has_type_helper<T>::value >
        {};


        // is a > b?
        template <size_t a, size_t b>
        struct is_greater : std::integral_constant<bool, (a > b)>
        {};


        //! A type to indicate that operation is not available for some combination of input types.
        struct operation_not_available;

        //! An empty type carrying no information, used whenever applicable.
        struct nothing {};


        //! Loop terminator.
        template <size_t Begin, class Func>
        inline void static_for_impl(Func, std::integral_constant<size_t, Begin>, std::integral_constant<size_t, Begin>)
        {
            // do nothing
        }

        //! A chain of func calls with each value from [Begin, End) range.
        template <size_t Begin, size_t End, class Func>
        inline typename std::enable_if<Begin != End, void>::type static_for_impl(Func func, std::integral_constant<size_t, Begin>, std::integral_constant<size_t, End>)
        {
            func(Begin);
            static_for_impl( func, std::integral_constant<size_t, Begin+1>(), std::integral_constant<size_t, End>() );
        }

        //! Trigger Func for each value from [Begin, End) range.
        template <size_t Begin, size_t End, class Func>
        inline void static_for(Func func)
        {
            static_for_impl( func, std::integral_constant<size_t, Begin>(), std::integral_constant<size_t, End>() );
        }

        //! Calls and returns a result of the decay memeber function (provided there's one).
        template <class T>
        inline auto decay(T&& t) -> decltype( t.decay() )
        {
            return t.decay();
        }

        //! Special case for nothing
        inline nothing decay(nothing)
        {
            return nothing();
        }

        //! If there's no decay function defined just return same object -- if it is a scalar.
        template <class T>
        inline typename std::enable_if< std::is_scalar< typename std::remove_reference<T>::type >::value, T>::type decay(T&& t)
        {
            return t;
        }


        //! Sum up all the arguments.
        template <size_t Head, size_t... Tail>
        struct accumulate
        {
            static const size_t value = Head + accumulate<Tail...>::value;
        };

        template <size_t Head>
        struct accumulate<Head>
        {
            static const size_t value = Head;
        };


        //! Does the sequence contain "What"?
        template <size_t What, size_t Head, size_t... Tail>
        struct contains
        {
            static const bool value = What == Head || contains<What, Tail...>::value;
        };

        template <size_t What, size_t Head>
        struct contains<What, Head>
        {
            static const bool value = What == Head;
        };


        //! Are all elements unique?
        template <size_t Head, size_t... Tail>
        struct are_unique
        {
            static const bool value = !contains<Head, Tail...>::value && are_unique<Tail...>::value;
        };

        template <size_t value1, size_t value2>
        struct are_unique<value1, value2>
        {
            static const bool value = value1 != value2;
        };


        //! Last template parameter.
        template <class Head, class... T>
        struct last
        {
            typedef typename last<T...>::type type;
        };

        template <class Head>
        struct last<Head>
        {
            typedef Head type;
        };
    }
}