#pragma once

#include <type_traits>

namespace swizzle
{
    namespace detail
    {
        //! Checks whether sum of sizes is greater/equal to VectorSize and whether it's the minimal
        //! i.e. redundant components are only allowed in last required size
        template <size_t VectorSize, size_t Size1, size_t Size2, size_t Size3, size_t Size4>
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

        //! A type to indicate that operation is not available for some combination of input types.
        struct operation_not_available;

        //! An empty type carrying no information, used whenever applicable.
        struct nothing {};


        //! A chain of func calls with each value from [Begin, End) range.
        template <size_t Begin, size_t End, class Func>
        inline void compile_time_for(Func func, std::integral_constant<size_t, Begin>, std::integral_constant<size_t, End>, typename std::enable_if< Begin != End, void>::type* = 0)
        {
            func(Begin);
            compile_time_for( func, std::integral_constant<size_t, Begin+1>(), std::integral_constant<size_t, End>() );
        }

        //! Loop terminator.
        template <size_t Begin, class Func>
        inline void compile_time_for(Func, std::integral_constant<size_t, Begin>, std::integral_constant<size_t, Begin>)
        {
            // do nothing
        }
    }
}