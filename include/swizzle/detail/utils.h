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
            static constexpr bool value = !std::is_void<decltype(test<T>(0))>::value;
        };

        template <typename T>
        struct has_type : std::integral_constant< bool, has_type_helper<T>::value >
        {};


        // is a > b?
        template <size_t TA, size_t TB>
        struct is_greater : std::integral_constant<bool, (TA > TB)>
        {};


        //! A type to indicate that operation is not available for some combination of input types.
        template <typename T, int TLine = 0>
        struct operation_not_available_t;
        struct operation_not_available;
        template <int> struct operation_not_available_n;

        //! An empty type carrying no information, used whenever applicable.
        struct nothing {};

        template <size_t TOffset, typename Func, size_t... TIndices>
        inline void static_for_impl(Func func, std::index_sequence<TIndices...>)
        {
            ((func(TOffset + TIndices)), ...);
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
        template <size_t THead, size_t... TTail>
        struct accumulate
        {
            static constexpr size_t value = THead + accumulate<TTail...>::value;
        };

        template <size_t THead>
        struct accumulate<THead>
        {
            static constexpr size_t value = THead;
        };

        template <template <class> class TPredicate, class THead, class... TTail>
        struct all
        {
            static constexpr bool value = TPredicate<THead>::value && all<TPredicate, TTail...>::value;
        };

        template <template <class> class TPredicate, class THead>
        struct all < TPredicate, THead >
        {
            static constexpr bool value = TPredicate<THead>::value;
        };

        //! Does the sequence contain "TWhat"?
        template <size_t TWhat, size_t THead, size_t... TTail>
        struct contains
        {
            static constexpr bool value = TWhat == THead || contains<TWhat, TTail...>::value;
        };

        template <size_t TWhat, size_t THead>
        struct contains<TWhat, THead>
        {
            static constexpr bool value = TWhat == THead;
        };


        //! Are all elements unique?
        template <size_t THead, size_t... TTail>
        struct are_unique
        {
            static constexpr bool value = !contains<THead, TTail...>::value && are_unique<TTail...>::value;
        };

        template <size_t TValue1, size_t TValue2>
        struct are_unique<TValue1, TValue2>
        {
            static constexpr bool value = TValue1 != TValue2;
        };


        //! Last template parameter.
        template <class THead, class... T>
        struct last
        {
            typedef typename last<T...>::type type;
        };

        template <class THead>
        struct last<THead>
        {
            typedef THead type;
        };

        template <class THead, class... T>
        using last_t = typename last<THead, T...>::type;


        template <size_t... TValues, size_t... Indices>
        auto take_n_resolver(std::index_sequence<Indices...>)
        {
            constexpr size_t values[] = { TValues... };
            return std::index_sequence<values[Indices]...>();
        }

        template <size_t TCount, size_t... TValues>
        using take_n = decltype(take_n_resolver<TValues...>(std::make_index_sequence<TCount>()));

        template <bool TCondition, typename T, int TLine = 0>
        using only_if = std::conditional_t<TCondition, T, detail::operation_not_available_t<T, TLine> >;

        //! A handly little type to make sure a func invoked before other destructors; just
        //! make sure it is the first one to be inherited.
        template <typename TCleanUpType>
        struct clean_up_before_other_destructors
        {
            std::function<void(TCleanUpType&)> cleanup;
            ~clean_up_before_other_destructors()
            {
                cleanup(*reinterpret_cast<TCleanUpType*>(this));
            }
        };

        template <typename T>
        constexpr decltype(auto) nonmasked(T& arg) noexcept {
            using actual_type = std::remove_reference_t<T>;
            if constexpr (std::is_fundamental_v<actual_type>)
            {
                return arg;
            }
            else
            {
                return static_cast<actual_type&&>(arg);
            }
        }

        template <typename T>
        constexpr auto narrowing_cast(T&& t)
        {
            if constexpr (std::is_same_v<double, std::remove_cv_t<std::remove_reference_t<T>>>)
                return static_cast<float>(t);
            else
                return t;
        }

        template <typename T, typename ...Arg>
        constexpr auto make_with_narrowing(Arg&& ...arg)
        {
            return T{ narrowing_cast(std::forward<Arg>(arg))... };
        }

    }
}