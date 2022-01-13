// CxxSwizzle (c) 2013-2021 Piotr Gwiazdowski
#pragma once

#include <cstdint>
#include <type_traits>
#include <iostream>
#include <memory>

#include <swizzle/detail/fwd.hpp>
#include <swizzle/detail/utils.hpp>


namespace swizzle 
{
    template <typename TElement, size_t TSize>
    struct array
    {
        TElement elems[TSize];

        constexpr int length() const noexcept 
        {
            return static_cast<int>(TSize);
        }

        constexpr int _cxxswizzle_func_length() const noexcept
        {
            return length();
        }

        constexpr TElement& operator[](int p) noexcept
        {
            return elems[p >= 0 && p < TSize ? p : 0];
        }

        constexpr const TElement& operator[](int p) const noexcept
        {
            return elems[p >= 0 && p < TSize ? p : 0];
        }
    };

    template <typename TElement>
    struct array_ptr
    {
        TElement* ptr;
        int size;

        template <size_t TSize>
        array_ptr(array<TElement, TSize>& arr) 
        {
            ptr = arr.elems;
            size = static_cast<int>(TSize);
        }

        constexpr int length() const noexcept
        {
            return size;
        }

        constexpr int _cxxswizzle_func_length() const noexcept
        {
            return length();
        }

        constexpr TElement& operator[](int p) noexcept
        {
            return ptr[p >= 0 && p < size ? p : 0];
        }

        constexpr const TElement& operator[](int p) const noexcept
        {
            return ptr[p >= 0 && p < size ? p : 0];
        }

        template <size_t TSize>
        operator array<TElement, TSize>() const
        {
            array<TElement, TSize> result;
            for (int i = 0; i < size; ++i) {
                result[i] = ptr[i];
            }
            return std::move(result);
        }

    };


    template <typename Dest = void, typename ...Arg>
    static constexpr auto make_array(Arg&& ...arg)
    {
        if constexpr (std::is_same<void, Dest>::value)
            return array<std::common_type_t<std::decay_t<Arg>...>, sizeof...(Arg)>{ { detail::narrowing_cast(std::forward<Arg>(arg))... }};
        else
            return array<Dest, sizeof...(Arg)>{{ detail::narrowing_cast(std::forward<Arg>(arg))... }};
    }

    template <typename TElement>
    class auto_array
    {
        const int size;
        std::unique_ptr<TElement[]> ptr;

    public:
        template <size_t TSize>
        auto_array(array<TElement, TSize>&& array) : size(static_cast<int>(TSize))
        {
            ptr = std::make_unique<TElement[]>(size);
            std::copy(std::begin(array.elems), std::end(array.elems), ptr.get());
        }

        constexpr TElement& operator[](int p) noexcept
        {
            return ptr[p >= 0 && p < size ? p : 0];
        }

        constexpr const TElement& operator[](int p) const noexcept
        {
            return ptr[p >= 0 && p < size ? p : 0];
        }

        constexpr int length() const noexcept
        {
            return size;
        }

        constexpr int _cxxswizzle_func_length() const noexcept
        {
            return length();
        }
    };
}