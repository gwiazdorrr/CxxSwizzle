// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <type_traits>
#include <cstddef>
#include <utility>
#include <functional>

namespace swizzle
{
    namespace detail
    {
        //! Removes reference and const/volatile
        template <class T>
        struct remove_reference_cv : std::remove_cv< typename std::remove_reference<T>::type > {};

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
        template <typename T, int line = 0>
        struct operation_not_available_t;
        struct operation_not_available;
        template <int> struct operation_not_available_n;

        //! An empty type carrying no information, used whenever applicable.
        struct nothing {};

        template <size_t Offset, typename Func, size_t... Index>
        inline void static_for_impl(Func func, std::index_sequence<Index...>)
        {
            ((func(Offset + Index)), ...);
        }

        //! Trigger Func for each value from [Begin, End) range.
        template <size_t Begin, size_t End, class Func>
        inline void static_for(Func func)
        {
            static_for_impl<Begin>(func, std::make_index_sequence<End - Begin>());
        }

        //! Calls and returns a result of the decay memeber function (provided there's one).
        template <class T>
        inline auto decay(T&& t) -> decltype(t.decay())
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

        template <template <class> class Predicate, class Head, class... Tail>
        struct all
        {
            static const bool value = Predicate<Head>::value && all<Predicate, Tail...>::value;
        };

        template <template <class> class Predicate, class Head>
        struct all < Predicate, Head >
        {
            static const bool value = Predicate<Head>::value;
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

        template <class Head, class... T>
        using last_t = typename last<Head, T...>::type;

        template <size_t Count, size_t... Left, size_t RightFirst, size_t... Right>
        constexpr auto take_n_resolver(std::index_sequence<Left...> left, std::index_sequence<RightFirst, Right...>)
        {
            if constexpr (Count == 1)
                return std::index_sequence<Left..., RightFirst> {};
            else 
                return take_n_resolver<Count - 1>(std::index_sequence<Left..., RightFirst>{}, std::index_sequence<Right...>{});
        }

        template <size_t Count, size_t... Values>
        using take_n = decltype(take_n_resolver<Count>(std::index_sequence<>{}, std::index_sequence<Values...>{}));

        template <bool Condition, typename T, int Line = 0>
        using only_if = std::conditional_t<Condition, T, detail::operation_not_available_t<T, Line> >;

        //! A handly little type to make sure a func invoked before other destructors; just
        //! make sure it is the first one to be inherited.
        template <typename TypeToCleanUp>
        struct clean_up_before_other_destructors
        {
            std::function<void(TypeToCleanUp&)> cleanup;
            ~clean_up_before_other_destructors()
            {
                cleanup(*reinterpret_cast<TypeToCleanUp*>(this));
            }
        };

    }
}