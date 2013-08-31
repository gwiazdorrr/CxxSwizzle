#pragma once

#include <type_traits>

namespace swizzle
{
    namespace detail
    {
        //! Checks whether sum of sizes is greater/equal to VectorSize and whether it's the minimal
        //! i.e. redundant components are only allowed in last required size
        template <size_t VectorSize, size_t Size1, size_t Size2=0, size_t Size3=0, size_t Size4=0>
        struct are_sizes_valid
        {
            static const bool value = 
                (VectorSize > Size1 || !Size2 && !Size3 && !Size4) &&
                (VectorSize > Size1 + Size2 || !Size3 && !Size4) &&
                (VectorSize > Size1 + Size2 + Size3 || !Size4) &&
                (VectorSize <= Size1 + Size2 + Size3 + Size4);
        };

        //! Removes reference and const/volatile
        template <class T>
        struct remove_reference_cv : std::remove_cv< typename std::remove_reference<T>::type > {};

        //! Detects presence of nested "typedef <something> type"
        template <typename T>
        struct has_type_helper {
        private:
            template <typename T1>
            static typename T1::type test(int);
            template <typename>
            static void test(...);
        public:
            static const bool value = !std::is_void<decltype(test<T>(0))>::value;
        };

        template <typename T>
        struct has_type : std::integral_constant< bool, has_type_helper<T>::value >
        {};

        template <size_t a, size_t b>
        struct is_greater_helper
        {
            static const bool value = a > b;
        };

        // is a > b?
        template <size_t a, size_t b>
        struct is_greater : std::integral_constant<bool, is_greater_helper<a, b>::value>
        {};

        //! A type to indicate that operation is not available for some combination of input types.
        struct operation_not_available;

        //! An empty type carrying no information, used whenever applicable.
        struct nothing {};


        //! A chain of func calls with each value from [Begin, End) range.
        template <size_t Begin, size_t End, class Func>
        inline void compile_time_for(Func func, std::integral_constant<size_t, Begin> = std::integral_constant<size_t, Begin>(), std::integral_constant<size_t, End> = std::integral_constant<size_t, End>(), typename std::enable_if< Begin != End, void>::type* = 0)
        {
            func(Begin);
            compile_time_for( func, std::integral_constant<size_t, Begin+1>(), std::integral_constant<size_t, End>() );
        }

        //! Loop terminator.
        template <size_t Begin, class Func>
        inline void compile_time_for(Func, std::integral_constant<size_t, Begin> = std::integral_constant<size_t, Begin>(), std::integral_constant<size_t, Begin> = std::integral_constant<size_t, Begin>())
        {
            // do nothing
        }
    }
}